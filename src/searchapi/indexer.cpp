#include <libfts/indexer.hpp>
#include <searchapi/indexer.h>

extern "C" {

TextIndexAccessor *index_accessor_new(const char *index_path) {
    try {
        // (＋_＋) ＼(〇_ｏ)／
        const libfts::BinaryData index(index_path);
        libfts::Header header(index.data());
        auto *accessor = new libfts::BinaryIndexAccessor(index.data(), header);
        // auto *accessor = new libfts::TextIndexAccessor(index_path);
        return reinterpret_cast<TextIndexAccessor *>(accessor);
    } catch (const libfts::AccessorException &ex) {
        return nullptr;
    } catch (const std::filesystem::__cxx11::filesystem_error &ex) {
        return nullptr;
    }
}

void index_accessor_delete(TextIndexAccessor *p_accessor) {
    auto *accessor =
        reinterpret_cast<libfts::BinaryIndexAccessor *>(p_accessor);
    delete accessor;
}
}