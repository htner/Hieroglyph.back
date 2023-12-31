/*-------------------------------------------------------------------------
 *
 * exec_state.cpp
 *		  FDW routines for parquet_s3_fdw
 *
 * Portions Copyright (c) 2021, TOSHIBA CORPORATION
 * Portions Copyright (c) 2018-2019, adjust GmbH
 *
 * IDENTIFICATION
 *		  contrib/parquet_s3_fdw/src/exec_state.cpp
 *
 *-------------------------------------------------------------------------
 */
#include "backend/sdb/common/pg_export.hpp"
#include "backend/access/parquet/reader_state.hpp"
#include "backend/access/parquet/heap.hpp"

#include <brpc/server.h>
#include <brpc/channel.h>
#include <butil/iobuf.h>
#include <butil/logging.h>
#include <sys/time.h>
#include <functional>
#include <list>


#if PG_VERSION_NUM < 110000
#define MakeTupleTableSlotCompat(tupleDesc) MakeSingleTupleTableSlot(tupleDesc)
#elif PG_VERSION_NUM < 120000
#define MakeTupleTableSlotCompat(tupleDesc) MakeTupleTableSlot(tupleDesc)
#else
#define MakeTupleTableSlotCompat(tupleDesc) MakeTupleTableSlot(tupleDesc, &TTSOpsVirtual)
#endif

/*
 * More compact form of common PG_TRY/PG_CATCH block which throws a c++
 * exception in case of errors.
 */
#define PG_TRY_INLINE(code_block, err) \
    do { \
        bool error = false; \
        PG_TRY(); \
        code_block \
        PG_CATCH(); { error = true; } \
        PG_END_TRY(); \
        if (error) { throw std::runtime_error(err); } \
    } while(0)


class MultifileExecutionStateS3 : public ParquetS3ReaderState {
private:
    struct FileRowgroups {
		uint64_t fileid_;
        std::string         filename;
        std::vector<int>    rowgroups;
    };

private:
    ParquetReader          *reader_;
    std::map<uint64_t, FileRowgroups> files;

    MemoryContext           cxt_;
    TupleDesc               tuple_desc;
	std::vector<bool>       fetched_col_;
    bool                    use_threads;
    bool                    use_mmap;

	std::string				dirname;
    Aws::S3::S3Client      *s3_client;

private:
    ParquetReader *get_next_reader() {
        ParquetReader *r;
		std::map<uint64_t, FileRowgroups>::iterator it;
		if (reader_ != nullptr) {
			it = files.upper_bound(reader_->id());
		} else {
			it = files.begin();
		}
		if (it == files.end()) {
			return nullptr;
		}
        r = CreateParquetReader(rel_, it->second.fileid_,
		 						it->second.filename.c_str(),
		 						tuple_desc, fetched_col_);
        r->SetRowgroupsList(it->second.rowgroups);
        r->SetOptions(use_threads, use_mmap);
		arrow::Status st;
        if (s3_client) {
            st = r->Open(dirname.c_str(), s3_client);
		} else {
            st = r->Open();
		}
		if (!st.ok()) {
			delete r;
			return nullptr;
		}

        return r;
    }

public:
    MultifileExecutionStateS3(MemoryContext cxt,
                            const char *dirname,
                            Aws::S3::S3Client *s3_client,
                            TupleDesc tuple_desc,
							const std::vector<bool>& fetched_col,
                            bool use_threads,
                            bool use_mmap)
        : reader_(nullptr), cxt_(cxt), tuple_desc(tuple_desc),
		  fetched_col_(fetched_col), use_threads(use_threads),
		  use_mmap(use_mmap), dirname(dirname), s3_client(s3_client)
    { }

    virtual ~MultifileExecutionStateS3() {
        if (reader_) {
            delete reader_;
		}
		if (s3_client) {
			//s3_client.close();
			delete s3_client;
		}
		reader_ = nullptr;
		MemoryContextDelete(cxt_);
    }

    bool next(TupleTableSlot *slot, bool fake=false) {
        ReadStatus  res;

        if (unlikely(reader_ == NULL)) {
			reader_ = this->get_next_reader();
            if (reader_ == NULL)
                return false;
        }

        res = reader_->Next(slot, fake);

        /* Finished reading current reader? Proceed to the next one */
        if (unlikely(res != RS_SUCCESS)) {
            while (true) {
                if (reader_) {
                    delete reader_;
				}
                reader_ = this->get_next_reader();
                if (reader_ == nullptr)
                    return false;
                res = reader_->Next(slot, fake);
                if (res == RS_SUCCESS) {
                    break;
				}
            }
        }

        if (res == RS_SUCCESS) {
			return true;
        } else {
			return false;
		}
    }

    bool fetch(ItemPointer tid, TupleTableSlot *slot, bool fake) {
		auto fileid = ItemPointerGetBlockNumberNoCheck(tid);
		if (reader_ != nullptr && (size_t)(reader_->id()) == (size_t)fileid) {
			return reader_->Fetch(ItemPointerGetOffsetNumberNoCheck(tid), slot);
		}

        ParquetReader *r;
		std::map<uint64_t, FileRowgroups>::iterator it = files.find(fileid);
		if (it == files.end()) {
			return false;
		}
        r = CreateParquetReader(rel_, it->second.fileid_, it->second.filename.c_str(), tuple_desc, fetched_col_);
        r->SetRowgroupsList(it->second.rowgroups);
        r->SetOptions(use_threads, use_mmap);
		arrow::Status st;
        if (s3_client)
            st = r->Open(dirname.c_str(), s3_client);
        else
            st = r->Open();
		if (!st.ok()) {
			delete r;
			return false;
		}

        reader_ = r;
		return reader_->Fetch(ItemPointerGetOffsetNumberNoCheck(tid), slot);
	} 

    void rescan(void) {
        reader_->Rescan();
    }

    void add_file(uint64_t fileid, const char *filename, List *rowgroups) {
        FileRowgroups   fr;
        ListCell       *lc;

        fr.filename = filename;
		fr.fileid_ = fileid;
        foreach (lc, rowgroups)
            fr.rowgroups.push_back(lfirst_int(lc));
        files[fileid] = fr;
    }

    size_t estimate_coord_size() {
		return 0;
    }
};

ParquetS3ReaderState*
create_parquet_execution_state(ReaderType reader_type,
							   MemoryContext reader_cxt,
							   const char *dirname,
							   Aws::S3::S3Client *s3_client,
							   Oid rel,
							   TupleDesc tuple_desc,
							   const std::vector<bool>& fetched_col,
							   bool use_threads,
							   bool use_mmap,
							   int32_t max_open_files) {
	ParquetS3ReaderState* result;
    switch (reader_type) {
        case RT_MULTI:
            result = new MultifileExecutionStateS3(reader_cxt, dirname,
			 							s3_client, tuple_desc, fetched_col,
										use_threads, use_mmap);
			break;
        default:
            throw std::runtime_error("unknown reader type");
    }
	result->SetRel(rel);
	return result;
}
