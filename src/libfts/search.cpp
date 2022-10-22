#include <libfts/search.hpp>

#include <algorithm>
#include <cmath>

namespace libfts {

static double
calculate_term_frequency(const Term &term, DocId docs, IndexAccessor &idx) {
    return static_cast<double>(
        idx.get_term_positions_in_document(term, docs).size());
}

static double calculate_document_frequency(const std::vector<DocId> &docs) {
    return static_cast<double>(docs.size());
}

static ScoreTable sort_by_score(const std::map<DocId, double> &score_table) {
    ScoreTable sorted_score_table;
    std::copy(
        score_table.begin(),
        score_table.end(),
        std::back_inserter<ScoreTable>(sorted_score_table));
    std::sort(
        sorted_score_table.begin(),
        sorted_score_table.end(),
        [](const auto &lhs, const auto &rhs) {
            return lhs.second != rhs.second ? lhs.second > rhs.second
                                            : lhs.first < rhs.first;
        });
    return sorted_score_table;
}

ScoreTable calculate_score(
    const std::string &query,
    const ParserConfiguration &config,
    IndexAccessor &index) {
    auto parsed_query = parse(query, config);
    std::map<DocId, double> score;
    for (const auto &word : parsed_query) {
        for (const auto &term : word.ngrams_) {
            auto docs = index.get_documents_by_term(term);
            auto doc_freq = calculate_document_frequency(docs);
            for (const auto &identifier : docs) {
                auto term_freq =
                    calculate_term_frequency(term, identifier, index);
                /* if this is the first time we access an item with this index,
                 * we initialize it with zero */
                if (score.find(identifier) == score.end()) {
                    score[identifier] = 0;
                }
                score[identifier] += term_freq *
                    log(static_cast<double>(index.get_document_count()) /
                        doc_freq);
            }
        }
    }
    return sort_by_score(score);
}

} // namespace libfts