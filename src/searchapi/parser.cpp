#include <libfts/parser.hpp>
#include <searchapi/parser.h>

extern "C" {
ParserConfiguration *
parser_configuration_load(ParserConfiguration *p_config, const char *path) {
    try {
        auto *config =
            reinterpret_cast<libfts::ParserConfiguration *>(p_config);
        auto loader = libfts::load_config(path);
        config->set_stop_words(loader.get_stop_words());
        config->set_ngram_length(
            {loader.get_min_ngram_length(), loader.get_max_ngram_length()});
        config->set_cutoff_factor(loader.get_cutoff_factor());
        return reinterpret_cast<ParserConfiguration *>(config);
    } catch (const libfts::ConfigurationException &ex) {
        return nullptr;
    };
}

ParserConfiguration *parser_configuration_new() {
    auto *config = new libfts::ParserConfiguration();
    return reinterpret_cast<ParserConfiguration *>(config);
}

void parser_configuration_delete(ParserConfiguration *p_config) {
    auto *config = reinterpret_cast<libfts::ParserConfiguration *>(p_config);
    delete config;
}
}