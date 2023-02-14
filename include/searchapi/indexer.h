#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BinaryIndexAccessor BinaryIndexAccessor;

BinaryIndexAccessor *index_accessor_new(const char *index_path);

void index_accessor_delete(BinaryIndexAccessor *p_accessor);

#ifdef __cplusplus
}
#endif