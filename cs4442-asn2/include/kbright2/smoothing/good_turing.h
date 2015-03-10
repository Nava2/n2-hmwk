#ifndef __GOOD_TURING_H
#define __GOOD_TURING_H

#include <vector>
#include <functional>

#include "../utils.h"
#include "../database.h"

namespace kbright2 {

namespace good_turing {

    namespace s1 {
        template <typename T>
        static typename Database<T>::ProbFunc fn() {
            return [](const Database<std::string>&, const NGram<std::string>&, const size_t) -> double {
                return 0.0; // no-op
            };
        }
    }

    namespace s2 {

    template <typename T>
    static typename Database<T>::ProbFunc fn(const size_t threshold,
                                                 std::function<double(const size_t)> powerFn) {
        return [&threshold, powerFn](const Database<T>& db,
                            const NGram<T>& that,
                            const size_t count) -> double {
            if (count >= threshold) {
                return NGramProbFunc::mle<T>(db, that, count);
            }

            if (count == 0) {
                return 1E-09;
            }

            // count < threshold
            return (count + 1.0) * powerFn(count + 1) / (powerFn(count) * db.vocabulary().size());
        };
    }

    template <typename T>
    static typename Database<T>::ProbFunc depFn() {
        return [](const Database<T>& db, const NGram<T>& that, const size_t) -> double {
            return db.prob(that);
        };
    }

    }

    template <typename T>
    static
    Database<T> createModel(const size_t N, const std::vector<T>& tokens, const size_t threshold) {
        const auto gt_s1 = DatabaseFactory::createFromTokens<T>(N, tokens,
                                                s1::fn<T>(), s1::fn<T>());

        // build vector<{ngram, count, freq
        const auto xys = gt_s1.rateToCountMapping();
        const auto powerFn = fitToPowerLaw(xys.first, xys.second);

        return gt_s1.recalculate(s2::fn<T>(threshold, powerFn), s2::depFn<T>());
    }

} // end namespace good_turing


} // end namespace kbright2

#endif // GOOD_TURING_H

