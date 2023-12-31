/*-------------------------------------------------------------------------
 *
 * modify_reader.cpp
 *      FDW routines for parquet_writer
 *
 * Portions Copyright (c) 2022, TOSHIBA CORPORATION
 *
 * IDENTIFICATION
 *      contrib/parquet_writer/src/modify_reader.cpp
 *
 *-------------------------------------------------------------------------
 */
#include "backend/access/parquet/parquet_writer.hpp"

#include <arrow/api.h>
#include <arrow/array.h>
#include <arrow/io/api.h>
#include <fcntl.h>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/schema.h>
#include <parquet/arrow/writer.h>
#include <parquet/exception.h>
#include <parquet/file_reader.h>
#include <parquet/statistics.h>

#include <butil/logging.h>
#include <brpc/server.h>
#include <brpc/channel.h>
#include <butil/iobuf.h>

#include "lake_service.pb.h"
#include "backend/sdb/common/common.hpp"

extern "C" {
#include "include/sdb/session_info.h"
}

#define TEMPORARY_DIR "/tmp/parquet_writer_temp/"

/**
 * @brief Create a modify parquet reader object
 *
 * @param filename target file name
 * @param cxt reader memory context
 * @param schema target file schema
 * @param is_new_file whether target file is new
 * @param reader_id reder id
 * @return ParquetWriter* modify parquet reader object
 */

std::shared_ptr<ParquetWriter> CreateParquetWriter(
    Oid rel, const char *filename, TupleDesc tuple_desc) {
  return std::make_shared<ParquetWriter>(rel, filename, tuple_desc);
}
/**
 * @brief Construct a new Modify Parquet Reader:: Modify Parquet Reader object
 *
 * @param filename target file name
 * @param cxt reader memory context
 * @param schema target file schema
 * @param is_new_file where target file is new
 * @param reader_id reder id
 */
ParquetWriter::ParquetWriter(Oid rel, const char *filename, TupleDesc tuple_desc) {
  filename_ = filename;
  is_delete_ = false;
  is_insert_ = false;
  lake_2pc_state_ = LAKE2PC_NULL;
  builder_ = std::make_shared<sdb::RecordBatchBuilder>(rel, tuple_desc);
  rel_id_ = rel;
  Assert(builder_ != nullptr);
}

/**
 *, @brief Destroy the Modify Parquet Reader:: Modify Parquet Reader object
 */
ParquetWriter::~ParquetWriter() {}

/**
 * @brief write arrow table as a parquet file to storage system
 *
 * @param dirname directory path
 * @param s3_client aws s3 client
 * @param table source table
 */
void ParquetWriter::ParquetWriteFile(const char *dirname,
                                     Aws::S3::S3Client *s3_client,
                                     const arrow::Table &table) {
  try {
    std::string local_path;

    /* create a local one */
    if (s3_client) {
      local_path = TEMPORARY_DIR;
      if (IS_S3_PATH(filename_.c_str())) /* remove 's3:/' */
        local_path += filename_.substr(5);
      else
        local_path += filename_;
    } else {
      local_path = filename_;
    }

    /* Get parent directory */
    std::string dir;
    const size_t last_slash_idx = local_path.rfind('/');
    if (std::string::npos != last_slash_idx) {
      dir = local_path.substr(0, last_slash_idx);
    }

    if (dir.empty()) {
      LOG(ERROR) << "parquet_writer: Unformed file path: " << local_path;
	  return;
    }

    /* Create parent directory if needed */
    if (!is_dir_exist(dir)) {
      make_path(dir);
    }

    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    PARQUET_ASSIGN_OR_THROW(outfile,
                            arrow::io::FileOutputStream::Open(local_path));
    const int64_t chunk_size =
        std::max(static_cast<int64_t>(1), table.num_rows());

    PARQUET_THROW_NOT_OK(parquet::arrow::WriteTable(
        table, arrow::default_memory_pool(), outfile, chunk_size));

    /* Upload to S3 system if needed */
    if (s3_client) {
      bool uploaded = parquet_upload_file_to_s3(
          dirname, s3_client, s3_filename_.c_str(), local_path.c_str());

      /* clean-up the local temporary file */
      /* delete temporary file */
      // std::remove(local_path.c_str());
      /* remove parent directory if it empty */
      remove_directory_if_empty(TEMPORARY_DIR);

      if (!uploaded) {
        LOG(ERROR) << "parquet_writer: upload file to s3 system failed!";
		return;
      }
    }
  } catch (const std::exception &e) {
    LOG(ERROR) << "parquet_writer: " << e.what();
  }
}

/**
 * @brief upload cached data to storage system
 *
 * @param dirname directory path
 * @param s3_client aws s3 client
 */
void ParquetWriter::Upload(const char *dirname, Aws::S3::S3Client *s3_client) {
  instr_time start, duration;

  std::shared_ptr<arrow::RecordBatch> record_batch;
  if (is_delete_) {
    // record_batch = record_batch_;
	return;
  } else if (is_insert_) {
    auto result = builder_->Finish();
    if (result == nullptr) {
	  LOG(WARNING) << filename_ << " file has been empty.";
      return;
    }
    record_batch = result;
  }

  std::vector<std::shared_ptr<arrow::RecordBatch>> batches;
  batches.push_back(record_batch);

  auto result = arrow::Table::FromRecordBatches(batches);

  if (!result.status().ok()) {
	LOG(WARNING) << filename_ << "file has been empty writer error"
	   << result.status().ToString();
    return;
  }
  std::shared_ptr<arrow::Table> table = *result;

  INSTR_TIME_SET_CURRENT(start);
  /* Upload file to the storage system */
  ParquetWriteFile(dirname, s3_client, *table);
  INSTR_TIME_SET_CURRENT(duration);
  INSTR_TIME_SUBTRACT(duration, start);
  LOG(INFO) << rel_name << "(" << rel_id_  << ") "
		<< s3_filename_ << " file has been uploaded in "
	<< duration.tv_sec << " seconds " << duration.tv_nsec/1000 << " microseconds.";
}

/**
 * @brief insert a record to cached parquet file data
 *
 * @param attrs inserted attributes
 * @param row_values inserted attribute values
 * @param is_nulls inserted attribute values is null
 * @return true successfully inserted
 */
bool ParquetWriter::ExecInsert(TupleTableSlot *slot) {
  auto status = builder_->AppendTuple(slot);
  is_insert_ = true;
  return status.ok();
}

/**
 * @brief delete a row by key column
 *
 * @param key_attrs key attributes
 * @param key_values key attributes
 * @return true if delete successfully
 */
bool ParquetWriter::ExecDelete(size_t pos) {
  try {
    deletes_.insert(pos);
    is_delete_ = true;
    return true;
  } catch (const std::exception &e) {
    LOG(ERROR) << "parquet_writer:" << e.what();
  }
  return false;
}

void ParquetWriter::PrepareUpload() {

	std::unique_ptr<brpc::Channel> channel;
	std::unique_ptr<sdb::Lake_Stub> stub;//(&channel);
	brpc::Controller cntl;
	channel = std::make_unique<brpc::Channel>();

	// LOG(ERROR) << "prepare upload";
	// Initialize the channel, NULL means using default options. 
	brpc::ChannelOptions options;
	options.protocol = "h2:grpc";
	//options.connection_type = "pooled";
	options.timeout_ms = 10000/*milliseconds*/;
	options.max_retry = 5;
	if (channel->Init("127.0.0.1", 10001, &options) != 0) {
		LOG(ERROR) << "PrepareUpload: Fail to initialize channel";
		return;
	}
	stub = std::make_unique<sdb::Lake_Stub>(channel.get());

	sdb::PrepareInsertFilesRequest request;
	//auto add_file  = request.add_add_files();
	//*add_file = filename_;

	// FIX_SDB: 
	request.set_dbid(thr_sess->session_cxt_.dbid_);
	request.set_sessionid(thr_sess->session_cxt_.sessionid_);
	request.set_rel(rel_id_);
	request.set_count(1);

	// LOG(INFO) << "prepare upload file " << request.DebugString();
	sdb::PrepareInsertFilesResponse response;
	//request.set_message("I'm a RPC to connect stream");
	stub->PrepareInsertFiles(&cntl, &request, &response, NULL);
	if (cntl.Failed()) {
		LOG(ERROR) << "Fail to PrepareInsertFiles, " << cntl.ErrorText();
		return;
	}
	if (response.files().size() == 0) {
		LOG(ERROR) << "Fail to PrepareInsertFiles, size == 0";
		return;
	}
	file_handler_ = response.files(0);
	s3_filename_ = std::to_string(file_handler_.file_id()) + ".parquet";
	file_id_ = file_handler_.file_id();
}

void ParquetWriter::CommitUpload() {
	if (old_filename_.size() == 0)  {
		return;
	}
	std::unique_ptr<brpc::Channel> channel;
	std::unique_ptr<sdb::Lake_Stub> stub;//(&channel);
	brpc::Controller cntl;
	channel = std::make_unique<brpc::Channel>();

	// Initialize the channel, NULL means using default options. 
	brpc::ChannelOptions options;
	options.protocol = "h2:grpc+proto";
	//options.connection_type = "pooled";
	options.timeout_ms = 10000/*milliseconds*/;
	options.max_retry = 5;
	if (channel->Init("127.0.0.1", 10001, &options) != 0) {
		LOG(ERROR) << "Fail to initialize channel";
		return;
	}
	stub = std::make_unique<sdb::Lake_Stub>(channel.get());

	sdb::DeleteFilesRequest request;
	//auto add_file = request.add_add_files();
	//add_file->set_file_id(0);
	//add_file->set_file_name(filename_);
	//add_file->set_space(1);
	//*add_file = filename_;
	request.set_dbid(thr_sess->session_cxt_.dbid_);
	request.set_sessionid(thr_sess->session_cxt_.sessionid_);
	request.set_rel(rel_id_);
	auto lakefile = request.add_remove_files();
	lakefile->set_file_id(file_id_);

	sdb::DeleteFilesResponse response;
	//request.set_message("I'm a RPC to connect stream");
	LOG(INFO) << "commit upload file " << request.DebugString();
	stub->DeleteFiles(&cntl, &request, &response, NULL);
	if (cntl.Failed()) {
		LOG(ERROR) << "Fail to UpdateFiles, " << cntl.ErrorText();
		return;
	}
}

void ParquetWriter::SetOldFilename(std::string filename) {
	old_filename_ = filename;
}

void ParquetWriter::SetRel(char *name, Oid id) { 
	rel_name = name; 
	rel_id_ = id;
}
