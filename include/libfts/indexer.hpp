#pragma once

#include <libfts/parser.hpp>

#include <map>
#include <string>
#include <vector>

namespace libfts {

using doc_id = size_t;
using pos = std::vector<size_t>;
using entry = std::map<doc_id, pos>;
using term = std::string;
using doc = std::string;

class Index {
protected:
    std::map<doc_id, doc> docs_;
    std::map<term, entry> entries_;

public:
    Index() = default;
    Index(std::map<doc_id, doc> docs, std::map<term, entry> entries)
        : docs_(std::move(docs)), entries_(std::move(entries)) {}
    std::map<doc_id, doc> get_docs() const { return docs_; }
    std::map<term, entry> get_entries() const { return entries_; }
};

class IndexBuilder : public Index {
public:
    void add_document(
        size_t document_id,
        const std::string &text,
        const ParserConfiguration &config);
    Index index();
};

struct TextIndexWriter {
    const size_t FIRST_THREE_BYTES = 6;
    void write(const std::string &path, const Index &index) const;
};

} // namespace libfts