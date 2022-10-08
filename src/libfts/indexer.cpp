#include <libfts/indexer.hpp>

#include <iostream>

namespace libfts {

void IndexBuilder::add_document(
    size_t document_id,
    const std::string &text,
    const ParserConfiguration &config) {
    docs_[document_id] = text;
    std::vector<ParsedString> parsed_text = parse(text, config);
    for (const auto &word : parsed_text) {
        for (const auto &term : word.ngrams_) {
            entries_[term][document_id].push_back(word.text_position_);
        }
    }
}

Index IndexBuilder::index() { return {docs_, entries_}; }

} // namespace libfts