#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BinaryData BinaryData;

typedef struct BinaryIndexAccessor BinaryIndexAccessor;

BinaryData *binary_data_map(const char *data_path);

void binary_data_unmap(BinaryData *p_data);

BinaryIndexAccessor *index_accessor_new(BinaryData *p_index);

void index_accessor_delete(BinaryIndexAccessor *p_accessor);

#ifdef __cplusplus
}
#endif