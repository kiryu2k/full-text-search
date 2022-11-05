#pragma once

#include <libfts/parser.hpp>

#include <map>

namespace libfts {

const size_t c_term_hash_size = 6;

class AccessorException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

using DocId = size_t;
using Pos = std::vector<size_t>;
using Entry = std::map<DocId, Pos>;
using Term = std::string;
using Doc = std::string;

class IndexBuilder;

class Index {
    friend IndexBuilder;
    std::map<DocId, Doc> docs_;
    std::map<Term, Entry> entries_;

public:
    Index() = default;
    Index(std::map<DocId, Doc> docs, std::map<Term, Entry> entries)
        : docs_(std::move(docs)), entries_(std::move(entries)) {}
    const std::map<DocId, Doc> &get_docs() const { return docs_; }
    const std::map<Term, Entry> &get_entries() const { return entries_; }
};

class IndexBuilder {
    Index index_;

public:
    void add_document(
        size_t document_id,
        const std::string &text,
        const ParserConfiguration &config);
    Index get_index() { return {index_.docs_, index_.entries_}; }
};

class IndexAccessor {
    Index index_;

public:
    explicit IndexAccessor(Index index) : index_(std::move(index)) {}
    Doc get_document_by_id(DocId identifier) const;
    std::vector<DocId> get_documents_by_term(const Term &term) const;
    Pos
    get_term_positions_in_document(const Term &term, DocId identifier) const;
    size_t get_document_count() const { return index_.get_docs().size(); }
};

struct TextIndexWriter {
    static void write(const std::filesystem::path &path, const Index &index);
};

struct TextIndexReader {
    static Index read(const std::filesystem::path &path);
};

std::string generate_hash(const std::string &Term);
bool is_added(const Index &index, const std::string &text);

} // namespace libfts