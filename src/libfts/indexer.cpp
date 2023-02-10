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

Doc IndexAccessor::get_document_by_id(DocId identifier) const {
    auto document = index_.get_docs().find(identifier);
    if (document == index_.get_docs().end()) {
        throw AccessorException(
            "failed to get the document by the specified identifier");
    }
    return document->second;
}

std::vector<DocId>
IndexAccessor::get_documents_by_term(const Term &term) const {
    std::vector<DocId> documents;
    auto entry = index_.get_entries().find(term);
    if (entry == index_.get_entries().end()) {
        throw AccessorException(
            "failed to get a list of document IDs for the specified term" +
            term);
    }
    for (const auto &[docs_id, position] : entry->second) {
        documents.push_back(docs_id);
    }
    return documents;
}

Pos IndexAccessor::get_term_positions_in_document(
    const Term &term, DocId identifier) const {
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

static BinaryBuffer write_docs_section(const Index &index) {
    BinaryBuffer buffer;
    const std::uint32_t docs_size = index.get_docs().size();
    buffer.write(&docs_size, sizeof(docs_size));
    for (const auto &[docs_id, docs] : index.get_docs()) {
        const std::uint8_t doc_size = docs.size() + 1;
        buffer.write(&doc_size, sizeof(doc_size));
        buffer.write(docs.data(), doc_size - 1);
    }
    return buffer;
}

static void serialize(
    Node *node,
    BinaryBuffer &buffer,
    std::unordered_map<Node *, Offsets> &node_to_offsets) {
    const std::uint32_t children_count = node->children_.size();
    buffer.write(&children_count, sizeof(children_count));
    for (const auto &child : node->children_) {
        const std::uint8_t letter = child.first;
        buffer.write(&letter, sizeof(letter));
    }
    for (std::size_t i = 0; i < children_count; ++i) {
        const std::uint32_t child_offset = 0xffffffff; // 0
        buffer.write(&child_offset, sizeof(child_offset));
    }
    const std::uint8_t is_leaf = node->is_leaf_;
    buffer.write(&is_leaf, sizeof(is_leaf));
    if (node->is_leaf_) {
        buffer.write(&node->entry_offset_, sizeof(node->entry_offset_));
    }
    for (const auto &child : node->children_) {
        node_to_offsets[child.second.get()].self_offset = buffer.size();
        node_to_offsets[node].children_offsets.push_back(buffer.size());
        serialize(child.second.get(), buffer, node_to_offsets);
    }
}

static void update_child_offsets(
    BinaryBuffer &buffer,
    std::unordered_map<Node *, Offsets> &node_to_offsets) {
    for (const auto &[node, offsets] : node_to_offsets) {
        std::size_t write_offset =
            offsets.self_offset + 4 + node->children_.size();
        for (const auto &child_offset : offsets.children_offsets) {
            buffer.write_to(&child_offset, sizeof(child_offset), write_offset);
            write_offset += sizeof(child_offset);
        }
    }
}

static BinaryBuffer write_dictionary_section(
    const Index &index, std::vector<std::uint32_t> &entry_offset) {
    BinaryBuffer buffer;
    Trie trie;
    std::size_t i = 0;
    for (const auto &[terms, entries] : index.get_entries()) {
        trie.insert(terms, entry_offset[i]);
        ++i;
    }
    std::unordered_map<Node *, Offsets> node_to_offsets;
    serialize(trie.root(), buffer, node_to_offsets);
    update_child_offsets(buffer, node_to_offsets);
    return buffer;
}

static BinaryBuffer write_entries_section(
    const Index &index, std::vector<std::uint32_t> &entry_offset) {
    BinaryBuffer buffer;
    entry_offset.push_back(0);
    for (const auto &[terms, entries] : index.get_entries()) {
        const std::uint32_t doc_count = entries.size();
        buffer.write(&doc_count, sizeof(doc_count));
        for (const auto &[doc_id, positions] : entries) {
            const std::uint32_t doc_offset = doc_id;
            const std::uint32_t pos_count = positions.size();
            buffer.write(&doc_offset, sizeof(doc_offset));
            buffer.write(&pos_count, sizeof(pos_count));
            for (const auto &position : positions) {
                const std::uint32_t pos = position;
                buffer.write(&pos, sizeof(pos));
            }
        }
        entry_offset.push_back(buffer.size()); //  - entry_offset.back()
    }
    return buffer;
}

static BinaryBuffer write_header_section() {
    BinaryBuffer buffer;
    const std::uint8_t section_count = 3;
    buffer.write(&section_count, sizeof(section_count));
    std::vector<std::string> section_name = {"dictionary", "entries", "docs"};
    for (const auto &section : section_name) {
        const std::uint8_t section_size = section.size() + 1;
        buffer.write(&section_size, sizeof(section_size));
        buffer.write(section.data(), section_size - 1);
        const std::uint32_t section_offset = 3;
        buffer.write(&section_offset, sizeof(section_offset));
    }
    return buffer;
}

void BinaryIndexWriter::write(
    const std::filesystem::path &path, const Index &index) {
    auto header_buffer = write_header_section();
    const auto docs_buffer = write_docs_section(index);
    std::vector<std::uint32_t> entry_offset;
    const auto entries_buffer = write_entries_section(index, entry_offset);
    std::uint32_t dictionary_offset = header_buffer.size();
    header_buffer.write_to(&dictionary_offset, sizeof(dictionary_offset), 12);
    auto dictionary_buffer = write_dictionary_section(index, entry_offset);
    std::uint32_t entries_offset = dictionary_offset + dictionary_buffer.size();
    header_buffer.write_to(&entries_offset, sizeof(entries_offset), 24);
    std::uint32_t docs_offset = entries_offset + entries_buffer.size();
    header_buffer.write_to(&docs_offset, sizeof(docs_offset), 33);
    std::ofstream file(path / "binary", std::ios::binary);
    // file.write(header_buffer.data_.data(), header_buffer.data_.size());
    // file.write(dictionary_buffer.data_.data(),
    // dictionary_buffer.data_.size()); file.write(entries_buffer.data_.data(),
    // entries_buffer.data_.size()); file.write(docs_buffer.data_.data(),
    // docs_buffer.data_.size());
    header_buffer.write_to_file(file);
    dictionary_buffer.write_to_file(file);
    entries_buffer.write_to_file(file);
    docs_buffer.write_to_file(file);
}

static void
parse_entry(const std::string &path, std::map<Term, Entry> &entries) {
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

static void
parse_document(const std::string &path, std::map<DocId, Doc> &docs) {
    std::fstream file(path, std::fstream::in);
    const auto last_delim = path.find_last_of('/') + 1;
    DocId doc_id = std::stoi(path.substr(last_delim));
    Doc document;
    std::getline(file, document);
    docs[doc_id] = document;
}

Index TextIndexReader::read(const std::filesystem::path &path) {
    std::map<DocId, Doc> docs;
    std::map<Term, Entry> entries;
    for (const auto &doc : std::filesystem::directory_iterator(path / "docs")) {
        parse_document(doc.path(), docs);
    }
    for (const auto &entry :
         std::filesystem::directory_iterator(path / "entries")) {
        parse_entry(entry.path(), entries);
    }
    return {docs, entries};
}

void BinaryBuffer::write(const void *data, size_t size) {
    const auto *data_start_address = static_cast<const char *>(data);
    std::copy(
        data_start_address,
        data_start_address + size,
        std::back_inserter(data_));
}

void BinaryBuffer::write_to(const void *data, size_t size, size_t offset) {
    const auto *data_start_address = static_cast<const char *>(data);
    auto offset_iter = data_.begin();
    std::advance(offset_iter, offset);
    data_.erase(offset_iter, offset_iter + size); // mb delete
    std::copy(
        data_start_address,
        data_start_address + size,
        std::inserter(data_, offset_iter)); // offset_iter
}

void BinaryBuffer::write_to_file(std::ofstream &file) const {
    file.write(data_.data(), data_.size());
}

std::string generate_hash(const std::string &term) {
    std::string hash_hex_term;
    picosha2::hash256_hex_string(term, hash_hex_term);
    return hash_hex_term.substr(0, c_term_hash_size);
}

void Trie::insert(const std::string &word, std::uint32_t entry_offset) {
    Node *current = &root_;
    for (const auto &symbol : word) {
        if (current->children_.find(symbol) == current->children_.end()) {
            current->children_[symbol] = std::make_unique<Node>();
        }
        current = current->children_[symbol].get();
    }
    current->entry_offset_ = entry_offset;
    if ((!current->is_leaf_) && (current != &root_)) {
        current->is_leaf_ = true;
    }
}

} // namespace libfts