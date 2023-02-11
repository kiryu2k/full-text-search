#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Index Index;

typedef struct TextIndexAccessor TextIndexAccessor;

Index *index_new();

void index_delete(Index *p_idx);

TextIndexAccessor *index_accessor_new(const char *index_path);

void index_accessor_delete(TextIndexAccessor *p_accessor);

void text_index_read(Index *p_idx, const char *path);

#ifdef __cplusplus
}
#endif