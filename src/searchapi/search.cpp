#include <libfts/search.hpp>
#include <searchapi/search.h>

#include <cstring>

extern "C" {
const char *search(
    const char *query, ParserConfiguration *p_config, IndexAccessor *p_index) {
    try {
        auto *config =
            reinterpret_cast<libfts::ParserConfiguration *>(p_config);
        auto *index = reinterpret_cast<libfts::IndexAccessor *>(p_index);
        // const auto result = libfts::search(query, *config, *index);
        const auto result = libfts::get_string_search_result(
            libfts::search(query, *config, *index));
        return strdup(result.c_str());
    } catch (const libfts::AccessorException &ex) {
        return nullptr;
    }
}

void search_result_delete(char *search_result) { free(search_result); }
}