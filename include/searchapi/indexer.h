#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TextIndexAccessor TextIndexAccessor;

TextIndexAccessor *index_accessor_new(const char *index_path);

void index_accessor_delete(TextIndexAccessor *p_accessor);

#ifdef __cplusplus
}
#endif