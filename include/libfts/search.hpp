#pragma once

#include <libfts/indexer.hpp>

namespace libfts {

struct Score {
    DocId document_id_ = 0;
    double score_ = 0;
    Doc text_;
};

using ScoreTable = std::vector<Score>;

std::string get_string_search_result(const ScoreTable &score_table);

ScoreTable search(
    const std::string &query,
    const ParserConfiguration &config,
    IndexAccessor &index);

} // namespace libfts