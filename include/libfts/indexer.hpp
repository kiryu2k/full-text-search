#pragma once

#include <libfts/parser.hpp>

#include <map>
#include <unordered_map>

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
    void set_docs(const std::map<DocId, Doc> &docs) { docs_ = docs; }
    void set_entries(const std::map<Term, Entry> &entries) {
        entries_ = entries;
    }
};

class IndexBuilder {
    Index index_;

public:
    void add_document(
        size_t document_id,
        const std::string &text,
        const ParserConfiguration &config);
    Index get_index() { return index_; }
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

class IndexWriter {
public:
    virtual ~IndexWriter() = default;
    virtual void
    write(const std::filesystem::path &path, const Index &index) = 0;
};

class TextIndexWriter : public IndexWriter {
public:
    void write(const std::filesystem::path &path, const Index &index) override;
};

class BinaryIndexWriter : public IndexWriter {
public:
    void write(const std::filesystem::path &path, const Index &index) override;
};

struct TextIndexReader {
    static Index read(const std::filesystem::path &path);
};

class BinaryBuffer {
private:
    std::vector<char> data_;

public:
    void write(const void *data, size_t size);
    void write_to(const void *data, size_t size, size_t offset);
    void write_to_file(std::ofstream &file) const;
    std::size_t size() const { return data_.size(); }
};

struct Node {
    std::map<char, std::unique_ptr<Node>> children_;
    std::uint32_t entry_offset_ = 0;
    bool is_leaf_ = false;
};

class Trie {
private:
    Node root_;

public:
    void insert(const std::string &word, std::uint32_t entry_offset);
    Node *root() { return &root_; }
};

struct Offsets {
    std::uint32_t self_offset = 0;
    std::vector<std::uint32_t> children_offsets;
};

// class BinaryReader {
// public:
//     const char *data_;
//     const char *current_;
//     BinaryReader(const char *buf);
//     void read(void *dest, std::size_t size);
// };

std::string generate_hash(const std::string &Term);

} // namespace libfts