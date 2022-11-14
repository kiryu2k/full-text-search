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

IndexAccessor *index_accessor_new(Index *p_idx) {
    auto *idx = reinterpret_cast<libfts::Index *>(p_idx);
    auto *accessor = new libfts::IndexAccessor(*idx);
    return reinterpret_cast<IndexAccessor *>(accessor);
}

void index_accessor_delete(IndexAccessor *p_accessor) {
    auto *accessor = reinterpret_cast<libfts::IndexAccessor *>(p_accessor);
    delete accessor;
}

Index *text_index_read(Index *p_idx, const char *path) {
    auto *idx = reinterpret_cast<libfts::Index *>(p_idx);
    auto reader = libfts::TextIndexReader::read(path);
    idx->set_docs(reader.get_docs());
    idx->set_entries(reader.get_entries());
    return reinterpret_cast<Index *>(idx);
}
}