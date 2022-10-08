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
public:
    std::map<doc_id, doc> docs_;
    std::map<term, entry> entries_;

public:
    Index() = default;
    Index(std::map<doc_id, doc> docs, std::map<term, entry> entries)
        : docs_(std::move(docs)), entries_(std::move(entries)) {}
};

class IndexBuilder : public Index {
public:
    void add_document(
        size_t document_id,
        const std::string &text,
        const ParserConfiguration &config);
    Index index();
};

class TextIndexWriter {
public:
    void write(std::string path, Index index);
};

} // namespace libfts