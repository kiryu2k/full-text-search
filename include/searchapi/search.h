#pragma once

#include <searchapi/indexer.h>
#include <searchapi/parser.h>

#ifdef __cplusplus
extern "C" {
#endif

const char *search(
    const char *query, ParserConfiguration *p_config, IndexAccessor *p_index);

void search_result_delete(char *search_result);
#ifdef __cplusplus
}
#endif