#include <libfts/indexer.hpp>
#include <searchapi/indexer.h>

extern "C" {

TextIndexAccessor *index_accessor_new(const char *index_path) {
    try {
        auto *accessor = new libfts::TextIndexAccessor(index_path);
        return reinterpret_cast<TextIndexAccessor *>(accessor);
    } catch (const libfts::AccessorException &ex) {
        return nullptr;
    } catch (const std::filesystem::__cxx11::filesystem_error &ex) {
        return nullptr;
    }
}

void index_accessor_delete(TextIndexAccessor *p_accessor) {
    auto *accessor = reinterpret_cast<libfts::TextIndexAccessor *>(p_accessor);
    delete accessor;
}
}