#ifndef __ADD_DELTA_H
#define __ADD_DELTA_H

#include <vector>
#include <string>

#include "../database.h"

namespace kbright2 {

namespace add_delta {

template <typename T>
static typename Database<T>::ProbFunc fn(const double delta) {
    return [delta](const Database<T>& db, const NGram<T>& that, const size_t count) -> double {
        return (count + delta) / (db.ngramCount(that.n()) + delta * std::pow(db.vocabulary().size(), that.n()));
    };
}

template <typename T>
static
const Database<T> createModel(const size_t N, const std::vector<T>& tokens, const double delta) {
    return DatabaseFactory::createFromTokens<T>(N, tokens, fn<T>(delta),
                                                        &NGramProbFunc::dependantProb<T>);
}

} // end namespace add_delta

} // end namespace kbright2


#endif // ADD_DELTA_H

