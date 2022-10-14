#include <libfts/indexer.hpp>

#include <picosha2/picosha2.h>

#include <fstream>
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

static std::string convert_entries(const term &term, const entry &entry) {
    std::string result = term + " " + std::to_string(entry.size()) + " ";
    for (const auto &[docs_id, position] : entry) {
        result += std::to_string(docs_id) + " " +
            std::to_string(position.size()) + " ";
        for (const auto &pos : position) {
            result += std::to_string(pos) + " ";
        }
    }
    return result;
}

void TextIndexWriter::write(const std::string &path, const Index &index) const {
    // std::filesystem::current_path(std::filesystem::temp_directory_path());
    // std::filesystem::create_directory(path + "docs/");
    // std::filesystem::create_directory(path + "entries/");
    for (const auto &[docs_id, docs] : index.get_docs()) {
        std::fstream file(
            path + "docs/" + std::to_string(docs_id), std::fstream::out);
        std::cout << "file.is_open() = " << file.is_open() << '\n';
        file << docs;
    }
    for (const auto &[terms, entries] : index.get_entries()) {
        std::string hash_hex_term;
        picosha2::hash256_hex_string(terms, hash_hex_term);
        hash_hex_term.resize(FIRST_THREE_BYTES);
        std::fstream file(path + "entries/" + hash_hex_term, std::fstream::out);
        std::cout << "file.is_open() = " << file.is_open() << '\n';
        file.seekp(0, std::ios_base::end);
        file << convert_entries(terms, entries);
    }
}

} // namespace libfts