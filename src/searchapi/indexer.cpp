#include <libfts/indexer.hpp>
#include <searchapi/indexer.h>

extern "C" {

Index *index_new() {
    auto *idx = new libfts::Index();
    return reinterpret_cast<Index *>(idx);
}

// static Index *index_new(
//     std::map<libfts::DocId, libfts::Doc> docs,
//     std::map<libfts::Term, libfts::Entry> entries) {
//     auto *idx = new libfts::Index(std::move(docs), std::move(entries));
//     return reinterpret_cast<Index *>(idx);
// }

void index_delete(Index *p_idx) {
    auto *idx = reinterpret_cast<libfts::Index *>(p_idx);
    delete idx;
}

TextIndexAccessor *index_accessor_new(const char *index_path) {
    auto *accessor = new libfts::TextIndexAccessor(index_path);
    return reinterpret_cast<TextIndexAccessor *>(accessor);
}

void index_accessor_delete(TextIndexAccessor *p_accessor) {
    auto *accessor = reinterpret_cast<libfts::TextIndexAccessor *>(p_accessor);
    delete accessor;
}

void text_index_read(Index *p_idx, const char *path) {
    try {
        auto *idx = reinterpret_cast<libfts::Index *>(p_idx);
        auto reader = libfts::TextIndexReader::read(path);
        idx->set_docs(reader.get_docs());
        idx->set_entries(reader.get_entries());
        p_idx = reinterpret_cast<Index *>(idx);
    } catch (const std::filesystem::filesystem_error &ex) {
        p_idx = nullptr;
    }
}
}