#include <libfts/search.hpp>

#include <cmath>

namespace libfts {

static size_t
calculate_term_frequency(const Term &term, DocId docs, IndexAccessor &idx) {
    return idx.get_term_positions_in_document(term, docs).size();
}

static size_t calculate_document_frequency(const std::vector<DocId> &docs) {
    return docs.size();
}

static double
calculate_score(size_t term_freq, size_t doc_freq, size_t doc_count) {
    return term_freq *
        log(static_cast<double>(doc_count) / static_cast<double>(doc_freq));
};

std::map<DocId, double> search(
    const std::string &query,
    const ParserConfiguration &config,
    IndexAccessor &index) {
    auto parsed_query = parse(query, config);
    std::map<DocId, double> score;
    /* 3 цикла.. мб сделать функцию, возвращающую вектор только из термов? */
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
                score[identifier] += calculate_score(
                    term_freq, doc_freq, index.get_document_count());
            }
        }
    }
    return score;
}

} // namespace libfts