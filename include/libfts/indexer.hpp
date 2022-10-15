#pragma once

#include <libfts/parser.hpp>

#include <map>
#include <string>
#include <vector>

namespace libfts {

const size_t FIRST_NECESSARY_BYTES = 6;

using doc_id = size_t;
using pos = std::vector<size_t>;
using entry = std::map<doc_id, pos>;
using term = std::string;
using doc = std::string;

class IndexBuilder;

class Index {
    friend IndexBuilder;
    std::map<doc_id, doc> docs_;
    std::map<term, entry> entries_;

public:
    Index() = default;
    Index(std::map<doc_id, doc> docs, std::map<term, entry> entries)
        : docs_(std::move(docs)), entries_(std::move(entries)) {}
    std::map<doc_id, doc> get_docs() const { return docs_; }
    std::map<term, entry> get_entries() const { return entries_; }
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

struct TextIndexWriter {
    void write(const std::string &path, const Index &index) const;
};

std::string generate_hash(std::string term);
void parse_entry(const std::string &path, std::map<term, entry> &entries);

} // namespace libfts