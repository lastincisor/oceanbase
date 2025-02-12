// Copyright (c) 2022-present Oceanbase Inc. All Rights Reserved.
// Author:
//   suzhi.yt <suzhi.yt@oceanbase.com>

#define USING_LOG_PREFIX CLIENT

#include "ob_table_load_define.h"

namespace oceanbase
{
namespace table
{

OB_SERIALIZE_MEMBER_SIMPLE(ObTableLoadFlag, flag_);

OB_SERIALIZE_MEMBER_SIMPLE(ObTableLoadConfig,
                           session_count_,
                           batch_size_,
                           max_error_row_count_,
                           flag_);

OB_SERIALIZE_MEMBER_SIMPLE(ObTableLoadSegmentID,
                           id_);

OB_SERIALIZE_MEMBER_SIMPLE(ObTableLoadTransId,
                           segment_id_,
                           trans_gid_);

OB_SERIALIZE_MEMBER_SIMPLE(ObTableLoadPartitionId,
                           partition_id_,
                           tablet_id_);

OB_SERIALIZE_MEMBER_SIMPLE(ObTableLoadLSIdAndPartitionId,
                           ls_id_,
                           part_tablet_id_);

OB_SERIALIZE_MEMBER_SIMPLE(ObTableLoadResultInfo,
                           rows_affected_,
                           records_,
                           deleted_,
                           skipped_,
                           warnings_);

OB_DEF_SERIALIZE(ObTableLoadSqlStatistics)
{
  int ret = OB_SUCCESS;
  OB_UNIS_ENCODE(table_stat_array_.count());
  for (int64_t i = 0; OB_SUCC(ret) && i < table_stat_array_.count(); i++) {
    if (table_stat_array_.at(i) != nullptr) {
      OB_UNIS_ENCODE(*table_stat_array_.at(i));
    }
  }
  OB_UNIS_ENCODE(col_stat_array_.count());
  for (int64_t i = 0; OB_SUCC(ret) && i < col_stat_array_.count(); i++) {
    if (col_stat_array_.at(i) != nullptr) {
      OB_UNIS_ENCODE(*col_stat_array_.at(i));
    }
  }
  return ret;
}

OB_DEF_DESERIALIZE(ObTableLoadSqlStatistics)
{
  int ret = OB_SUCCESS;
  reset();
  int64_t size = 0;
  OB_UNIS_DECODE(size)
  for (int64_t i = 0; OB_SUCC(ret) && i < size; ++i) {
    ObOptTableStat table_stat;
    ObOptTableStat *copied_table_stat = nullptr;
    if (OB_FAIL(table_stat.deserialize(buf, data_len, pos))) {
      LOG_WARN("deserialize datum store failed", K(ret), K(i));
    } else {
      int64_t size = table_stat.size();
      char *new_buf = nullptr;
      if (OB_ISNULL(new_buf = static_cast<char *>(allocator_.alloc(size)))) {
        ret = OB_ALLOCATE_MEMORY_FAILED;
        OB_LOG(WARN, "fail to allocate buffer", KR(ret), K(size));
      } else if (OB_FAIL(table_stat.deep_copy(new_buf, size, copied_table_stat))) {
        OB_LOG(WARN, "fail to copy table stat", KR(ret));
      } else if (OB_FAIL(table_stat_array_.push_back(copied_table_stat))) {
        OB_LOG(WARN, "fail to add table stat", KR(ret));
      }
      if (OB_FAIL(ret)) {
        if (copied_table_stat != nullptr) {
          copied_table_stat->~ObOptTableStat();
          copied_table_stat = nullptr;
        }
        if(new_buf != nullptr) {
          allocator_.free(new_buf);
          new_buf = nullptr;
        }
      }
    }
  }
  size = 0;
  OB_UNIS_DECODE(size)
  for (int64_t i = 0; OB_SUCC(ret) && i < size; ++i) {
    ObArenaAllocator allocator;
    ObOptColumnStat col_stat(allocator);
    ObOptColumnStat *copied_col_stat = nullptr;
    if (OB_FAIL(col_stat.deserialize(buf, data_len, pos))) {
      LOG_WARN("deserialize datum store failed", K(ret), K(i));
    } else {
      int64_t size = col_stat.size();
      char *new_buf = nullptr;
      if (OB_ISNULL(new_buf = static_cast<char *>(allocator_.alloc(size)))) {
        ret = OB_ALLOCATE_MEMORY_FAILED;
        OB_LOG(WARN, "fail to allocate buffer", KR(ret), K(size));
      } else if (OB_FAIL(col_stat.deep_copy(new_buf, size, copied_col_stat))) {
        OB_LOG(WARN, "fail to copy table stat", KR(ret));
      } else if (OB_FAIL(col_stat_array_.push_back(copied_col_stat))) {
        OB_LOG(WARN, "fail to add table stat", KR(ret));
      }
      if (OB_FAIL(ret)) {
        if (copied_col_stat != nullptr) {
          copied_col_stat->~ObOptColumnStat();
          copied_col_stat = nullptr;
        }
        if(new_buf != nullptr) {
          allocator_.free(new_buf);
          new_buf = nullptr;
        }
      }
    }
  }
  return ret;
}

OB_DEF_SERIALIZE_SIZE(ObTableLoadSqlStatistics)
{
  int ret = OB_SUCCESS;
  int64_t len = 0;
  OB_UNIS_ADD_LEN(table_stat_array_.count());
  for (int64_t i = 0; OB_SUCC(ret) && i < table_stat_array_.count(); i++) {
    if (table_stat_array_.at(i) != nullptr) {
      OB_UNIS_ADD_LEN(*table_stat_array_.at(i));
    }
  }
  OB_UNIS_ADD_LEN(col_stat_array_.count());
  for (int64_t i = 0; OB_SUCC(ret) && i < col_stat_array_.count(); i++) {
    if (col_stat_array_.at(i) != nullptr) {
      OB_UNIS_ADD_LEN(*col_stat_array_.at(i));
    }
  }
  return len;
}

}  // namespace table
}  // namespace oceanbase
