#include <libfts/indexer.hpp>

#include <fmt/format.h>
#include <picosha2/picosha2.h>

#include <fstream>

namespace libfts {

void IndexBuilder::add_document(
    size_t document_id,
    const std::string &text,
    const ParserConfiguration &config) {
    /* if the document with this id has not yet been added, then add it,
     * otherwise make a return */
    if (index_.docs_.find(document_id) != index_.docs_.end()) {
        return;
    }
    index_.docs_[document_id] = text;
    std::vector<ParsedString> parsed_text = parse(text, config);
    for (const auto &word : parsed_text) {
        for (const auto &term : word.ngrams_) {
            index_.entries_[term][document_id].push_back(word.text_position_);
        }
    }
}

Doc IndexAccessor::get_document_by_id(DocId identifier) {
    auto document = index_.get_docs().find(identifier);
    if (document == index_.get_docs().end()) {
        throw AccessorException(
            "failed to get the document by the specified identifier");
    }
    return document->second;
}

std::vector<DocId> IndexAccessor::get_documents_by_term(const Term &term) {
    std::vector<DocId> documents;
    auto entry = index_.get_entries().find(term);
    if (entry == index_.get_entries().end()) {
        throw AccessorException(
            "failed to get a list of document IDs for the specified term");
    }
    for (const auto &[docs_id, position] : entry->second) {
        documents.push_back(docs_id);
    }
    return documents;
}

Pos IndexAccessor::get_term_positions_in_document(
    const Term &term, DocId identifier) {
    auto entry = index_.get_entries().find(term);
    if (entry == index_.get_entries().end()) {
        throw AccessorException(
            "failed to get a document for the specified term");
    }
    auto position = entry->second.find(identifier);
    if (position == entry->second.end()) {
        throw AccessorException(
            "failed to get a list of term positions in documents");
    }
    return position->second;
}

static std::string convert_entries(const Term &term, const Entry &entry) {
    std::string result = fmt::format("{} {} ", term, entry.size());
    for (const auto &[docs_id, position] : entry) {
        result += fmt::format("{} {} ", docs_id, position.size());
        for (const auto &pos : position) {
            result += fmt::format("{} ", pos);
        }
    }
    return result;
}

void TextIndexWriter::write(
    const std::filesystem::path &path, const Index &index) {
    std::filesystem::create_directories(path / "docs");
    std::filesystem::create_directories(path / "entries");
    for (const auto &[docs_id, docs] : index.get_docs()) {
        std::ofstream file(path / "docs" / std::to_string(docs_id));
        file << docs;
    }
    for (const auto &[terms, entries] : index.get_entries()) {
        std::string hash_hex_term = generate_hash(terms);
        std::ofstream file(path / "entries" / hash_hex_term);
        file << convert_entries(terms, entries);
    }
}

std::string generate_hash(const std::string &term) {
    std::string hash_hex_term;
    picosha2::hash256_hex_string(term, hash_hex_term);
    return hash_hex_term.substr(0, c_term_hash_size);
}

void parse_entry(const std::string &path, std::map<Term, Entry> &entries) {
    std::fstream file(path, std::fstream::in);
    std::string term;
    file >> term;
    size_t doc_count = 0;
    file >> doc_count;
    Entry entry;
    for (size_t i = 0; i < doc_count; ++i) {
        size_t document_id = 0;
        file >> document_id;
        Pos position;
        size_t pos_count = 0;
        file >> pos_count;
        for (size_t j = 0; j < pos_count; ++j) {
            size_t pos_num = 0;
            file >> pos_num;
            position.push_back(pos_num);
        }
        entry[document_id] = position;
    }
    entries[term] = entry;
}

} // namespace libfts