#include <libfts/parser.hpp>
#include <searchapi/parser.h>

extern "C" {

ParserConfiguration *parser_configuration_new(const char *path) {
    try {
        const auto config = libfts::load_config(path);
        return reinterpret_cast<ParserConfiguration *>(
            new libfts::ParserConfiguration(
                config.get_stop_words(),
                {config.get_min_ngram_length(), config.get_max_ngram_length()},
                config.get_cutoff_factor()));
    } catch (const libfts::ConfigurationException &ex) {
        return nullptr;
    };
}

void parser_configuration_delete(ParserConfiguration *p_config) {
    auto *config = reinterpret_cast<libfts::ParserConfiguration *>(p_config);
    delete config;
}
}