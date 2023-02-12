#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ParserConfiguration ParserConfiguration;

ParserConfiguration *parser_configuration_new(const char *path);

void parser_configuration_delete(ParserConfiguration *p_config);

#ifdef __cplusplus
}
#endif