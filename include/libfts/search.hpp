#pragma once

#include <libfts/indexer.hpp>

namespace libfts {

/* because of the std::map device, it can't be applied sorting by value, so
 * let's use vector of pairs of DocId and double, which can be sorted */
using ScoreTable = std::vector<std::pair<DocId, double>>;

std::string
get_string_result(const ScoreTable &score_table, IndexAccessor &index);

ScoreTable calculate_score(
    const std::string &query,
    const ParserConfiguration &config,
    IndexAccessor &index);

} // namespace libfts