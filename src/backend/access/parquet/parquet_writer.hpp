/*-------------------------------------------------------------------------

 * modify_reader.hpp
 *        FDW routines for parquet_s3_fdw
 *
 * Portions Copyright (c) 2022, TOSHIBA CORPORATION
 *
 * IDENTIFICATION
 *        contrib/parquet_s3_fdw/src/modify_reader.hpp
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARQUET_WRITER_HPP__AAA
#define PARQUET_WRITER_HPP__AAA
#pragma once

#include "backend/access/parquet/parquet_s3/parquet_s3.hpp"
#include "backend/sdb/arrow/recordbatch_builder.hpp"
#include "backend/access/parquet/common.hpp"

#include <arrow/api.h>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <parquet/arrow/reader.h>

#include "kvpair.pb.h"

/*
 * ParquetWriter
 *      - Read parquet file and cache this value
 *      - Overwrite parquet file by cache data
 *      - Create new file from given file schema
 */
enum Lake2PCState {
  LAKE2PC_NULL = 0,
  LAKE2PC_PREPARE = 1,
  LAKE2PC_COMMIT_WRITE = 2,
};

class ParquetWriter {
 private:
  /* remove row in idx in cache data */
  void RemoveRow(size_t idx);

 public:
  ParquetWriter(Oid rel, const char *filename, TupleDesc tuple_desc);

  ~ParquetWriter();

  size_t DataSize() { return data_size_; }

  /* execute insert a postgres slot */
  bool ExecInsert(TupleTableSlot *slot);

  /* delete a record by key column values */
  bool ExecDelete(size_t pos);

  /* create new parquet file and overwrite to storage system */
  void Upload(const char *dirname, Aws::S3::S3Client *s3_client);

  void PrepareUpload();
  void CommitUpload();

  void ParquetWriteFile(const char *dirname, Aws::S3::S3Client *s3_client,
                        const arrow::Table &table);

  void SetOldFilename(std::string filename);

  const std::string GetOldFilename() {
    return s3_filename_;
  }

  void SetRel(char *name, Oid id);

  uint64_t FileId() {
    return file_id_;
  }

  void SetFileId(uint64 file_id) {
    file_id_ = file_id;
  }

  const std::unordered_set<size_t> Deletes() {
    return deletes_;
  }

 private:
  /* column num */
  size_t column_num_ = 0;
  size_t data_size_ = 0;

  /* true if cache data has been modified */
  bool is_insert_ = false;
  bool is_delete_ = false;

  uint64_t file_id_ = 0;
  std::string rel_name;
  Oid rel_id_;
  std::string old_filename_;
  std::string filename_;
  sdb::LakeFile file_handler_;
  std::string s3_filename_;
  std::vector<std::string> column_names_;
  std::unordered_set<size_t> deletes_;
  /* schema of target file */
  std::shared_ptr<arrow::RecordBatch> record_batch_;
  std::shared_ptr<sdb::RecordBatchBuilder> builder_;

  Lake2PCState lake_2pc_state_;
};

std::shared_ptr<ParquetWriter> CreateParquetWriter(
    Oid rel, const char *filename, TupleDesc tuple_desc);

#endif
