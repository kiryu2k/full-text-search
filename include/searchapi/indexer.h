#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Index Index;

typedef struct IndexAccessor IndexAccessor;

Index *index_new();

void index_delete(Index *p_idx);

IndexAccessor *index_accessor_new(Index *p_idx);

void index_accessor_delete(IndexAccessor *p_accessor);

Index *text_index_read(Index *p_idx, const char *path);

#ifdef __cplusplus
}
#endif