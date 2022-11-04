#include <libfts/absolutepath.hpp>
#include <libfts/indexer.hpp>
#include <libfts/parser.hpp>

#include <gtest/gtest.h>

TEST(IndexerTest, AddMultipleDocuments) {
    const auto config =
        libfts::load_config(c_absolute_path / "ParserConfig.json");
    libfts::IndexBuilder idx;
    idx.add_document(199903, "The Matrix matrix awwww", config);
    idx.add_document(200305, "The Matrix Reloaded", config);
    idx.add_document(200311, "The Matrix Revolutions", config);
    libfts::TextIndexWriter::write(c_absolute_path / "index", idx.get_index());
    idx.add_document(200458, "The Matrix in the red bottoms MATR", config);
    libfts::TextIndexWriter::write(c_absolute_path / "index", idx.get_index());
    std::map<libfts::Term, libfts::Entry> entry;
    libfts::parse_entry(
        c_absolute_path / "index/entries" / libfts::generate_hash("matrix"),
        entry);
    std::map<libfts::Term, libfts::Entry> expected_entry;
    expected_entry.insert(
        {"matrix",
         {{199903, {0, 1}}, {200305, {0}}, {200311, {0}}, {200458, {0}}}});
    EXPECT_EQ(entry, expected_entry);
    std::map<libfts::DocId, libfts::Doc> document;
    libfts::parse_document(c_absolute_path / "index/docs/200305", document);
    std::map<libfts::DocId, libfts::Doc> expected_document;
    expected_document.insert({200305, "The Matrix Reloaded"});
    EXPECT_EQ(document, expected_document);
    libfts::IndexBuilder idx2;
    idx2.read(c_absolute_path / "index");
    EXPECT_EQ(idx2.get_index().get_docs(), idx.get_index().get_docs());
    EXPECT_EQ(idx2.get_index().get_entries(), idx.get_index().get_entries());
}

TEST(IndexerTest, AddDocumentWithExistingId) {
    const auto config =
        libfts::load_config(c_absolute_path / "ParserConfig.json");
    libfts::IndexBuilder idx;
    idx.add_document(101, "On Hawaii island", config);
    idx.add_document(101, "Mirror, mirror, the reflection of the law", config);
    std::map<libfts::DocId, libfts::Doc> expected_docs = {
        {101, "On Hawaii island"}};
    std::map<libfts::Term, libfts::Entry> expected_entries = {
        {"haw", {{101, libfts::Pos{0}}}},
        {"hawa", {{101, libfts::Pos{0}}}},
        {"hawai", {{101, libfts::Pos{0}}}},
        {"hawaii", {{101, libfts::Pos{0}}}},
        {"isl", {{101, libfts::Pos{1}}}},
        {"isla", {{101, libfts::Pos{1}}}},
        {"islan", {{101, libfts::Pos{1}}}},
        {"island", {{101, libfts::Pos{1}}}}};
    EXPECT_EQ(idx.get_index().get_docs(), expected_docs);
    EXPECT_EQ(idx.get_index().get_entries(), expected_entries);
}

TEST(IndexerTest, GetDocumentById) {
    const auto config =
        libfts::load_config(c_absolute_path / "ParserConfig.json");
    libfts::IndexBuilder idx;
    idx.add_document(100, "The Ides of March", config);
    idx.add_document(101, "The Place Beyond the Pines", config);
    idx.add_document(102, "All Good Things", config);
    try {
        libfts::IndexAccessor accessor(idx.get_index());
        EXPECT_EQ(accessor.get_document_by_id(100), "The Ides of March");
        EXPECT_EQ(accessor.get_document_by_id(102), "All Good Things");
        EXPECT_EQ(
            accessor.get_document_by_id(101), "The Place Beyond the Pines");
    } catch (libfts::AccessorException &ex) {
    };
}

TEST(IndexerTest, GetDocumentsByTerm) {
    const auto config =
        libfts::load_config(c_absolute_path / "ParserConfig.json");
    libfts::IndexBuilder idx;
    idx.add_document(100, "Hello World", config);
    idx.add_document(101, "Bye World", config);
    idx.add_document(102, "Hello Earth", config);
    try {
        libfts::IndexAccessor accessor(idx.get_index());
        std::vector<libfts::DocId> expected_docs{100, 101};
        EXPECT_EQ(accessor.get_documents_by_term("world"), expected_docs);
        expected_docs = {100, 102};
        EXPECT_EQ(accessor.get_documents_by_term("hello"), expected_docs);
        expected_docs = {101};
        EXPECT_EQ(accessor.get_documents_by_term("bye"), expected_docs);
    } catch (libfts::AccessorException &ex) {
    };
}