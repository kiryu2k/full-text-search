#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ParserConfiguration ParserConfiguration;

void parser_configuration_load(ParserConfiguration *p_config, const char *path);

ParserConfiguration *parser_configuration_new();

void parser_configuration_delete(ParserConfiguration *p_config);

#ifdef __cplusplus
}
#endif