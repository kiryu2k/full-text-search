#include <libfts/indexer.hpp>
#include <libfts/parser.hpp>

#include <gtest/gtest.h>

#include <iostream>

TEST(IndexerTest, Indexer) {
    libfts::ParserConfiguration config =
        libfts::load_config("../../../ParserConfig.json");
    libfts::IndexBuilder idx;
    idx.add_document(199903, "The Matrix matrix awwww", config);
    idx.add_document(200305, "The Matrix Reloaded", config);
    idx.add_document(200311, "The Matrix Revolutions", config);
    libfts::Index res = idx.index();
    std::cout << "id\tdocs\n";
    for (const auto &[docs_id, docs] : res.docs_) {
        std::cout << docs_id << " " << docs << "\n";
    }
    std::cout << "term\tentries\n";
    for (const auto &[terms, entries] : res.entries_) {
        std::cout << terms << "\t{";
        for (const auto &[docs_id, position] : entries) {
            std::cout << docs_id << ": [";
            for (const auto &pos : position) {
                std::cout << pos << " ";
            }
            std::cout << "], ";
        }
        std::cout << "}\n";
    }
}