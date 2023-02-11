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

using SectionName = std::string;
using SectionOffset = std::uint32_t;

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
public:
    virtual ~IndexAccessor() = default;
    virtual Doc get_document_by_id(DocId identifier) const = 0;
    virtual std::size_t get_document_count() const = 0;
};

class TextIndexAccessor : public IndexAccessor {
private:
    std::filesystem::path index_path_;

public:
    explicit TextIndexAccessor(std::filesystem::path index_path);
    Doc get_document_by_id(DocId identifier) const override;
    std::size_t get_document_count() const override;
    std::vector<DocId> get_documents_by_term(const Term &term) const;
    Pos
    get_term_positions_in_document(const Term &term, DocId identifier) const;
};

class Header {
private:
    std::uint8_t section_count_;
    std::unordered_map<SectionName, SectionOffset> sections_;

public:
    explicit Header(const char *data);
    std::uint8_t section_count() const { return section_count_; }
    SectionOffset section_offset(const SectionName &name) const;
};

class DictionaryAccessor {
private:
    const char *data_;

public:
    explicit DictionaryAccessor(const char *data) : data_(data) {}
    std::uint32_t retrieve(const std::string &word);
};

class EntriesAccessor {
private:
    const char *data_;

public:
    explicit EntriesAccessor(const char *data) : data_(data) {}
    Entry get_term_infos(std::uint32_t entry_offset);
};

class DocumentsAccessor {
private:
    const char *data_;

public:
    explicit DocumentsAccessor(const char *data) : data_(data) {}
    Doc get_document_by_id(DocId identifier) const;
    std::size_t get_document_count() const;
};

class BinaryIndexAccessor : public IndexAccessor {
private:
    const char *data_;
    Header header_;

public:
    BinaryIndexAccessor(const char *data, Header &header);
    std::uint32_t retrieve(const std::string &word);
    Entry get_term_infos(std::uint32_t entry_offset);
    Doc get_document_by_id(DocId identifier) const override;
    std::size_t get_document_count() const override;
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
    std::uint8_t is_leaf_ = 0;
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

class BinaryReader {
private:
    const char *data_;
    const char *current_;

public:
    explicit BinaryReader(const char *buf) : data_(buf), current_(buf) {}
    void read(void *dest, std::size_t size);
    const char *current() const { return current_; }
    void move(std::size_t size) { current_ += size; }
    void move_back_to_start() { current_ = data_; }
};

class BinaryData {
private:
    char *data_;
    std::size_t size_;

public:
    // std::vector<char> data_;
    explicit BinaryData(const std::filesystem::path &index_dir);
    ~BinaryData();
    char *data() const { return data_; }
    std::size_t size() const { return size_; }
};

std::string generate_hash(const std::string &Term);

} // namespace libfts