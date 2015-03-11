#ifndef __UTILS_H
#define __UTILS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

#include "fileRead.h"

namespace kbright2 {
 
/**
 * Reads sentences looking for the "<END>" token.
 * @param tokens Tokens to parse through.
 * @return unordered_set of sentences including "<END>"
 */
std::vector<std::vector<std::string> > parseSentences(const std::vector<std::string> &tokens) {
    using namespace std;

    vector<vector<std::string>> sentences;
    vector<std::string> sentence;

    for (const auto& token: tokens) {
        sentence.push_back(token);
        if (token == "<END>") {
            sentences.emplace_back(std::move(sentence));
            sentence.clear();
        }
    }

    return sentences;
}

/**
 * Reads sentences looking for the "<END>" token.
 * @param tokens Tokens to parse through.
 * @return unordered_set of sentences including "<END>"
 */
std::vector<std::vector<char> > parseSentences(const std::vector<char> &tokens, const size_t len) {
    std::vector<std::vector<char> > sentences;
    sentences.reserve(tokens.size() / len + 1);

//    cout << std::distance(tokens.begin(), tokens.end()) << " wat" << endl;
    for (auto cit = tokens.begin(); cit != tokens.end() && (std::distance(cit, tokens.end()) > len) ; cit += len) {
        sentences.push_back(std::vector<char>(cit, cit + len));
    }

    sentences.shrink_to_fit();

    return sentences;
}

/**
 * Pulls all values out of an arbitrary `unordered_map`
 * @param map Supplier for values
 * @return vector of values
 */
template <typename K, typename V>
const std::vector<V> getValues(const std::unordered_map<K, V>& map) {
    std::vector<V> values;
    values.reserve(map.size());

    for (const auto& p: map) {
        values.push_back(p.second);
    }

    return values;
}

/**
 * @brief fitToPowerLaw Computes a power-law function using the math here:
 * http://mathworld.wolfram.com/LeastSquaresFittingPowerLaw.html and returns a lamda that will calculate `y` for any
 * `r`.
 * @param xs x values
 * @param ys y values
 * @return Functional object that accepts a rate, and returns a "count" curved
 */
std::function<const double(const size_t)> fitToPowerLaw(const std::vector<size_t>& xs,
                                                        const std::vector<size_t>& ys) {
    assert(xs.size() == ys.size());

    const auto N = xs.size();

    std::vector<double> lnx, lny, lnxlny, lnx2, lny2;
    std::vector<std::vector<double>* > vecs{&lnx, &lny, &lnxlny, &lnx2, &lny2};
    for (auto& v : vecs) v->reserve(xs.size());

    for (const auto _x: xs) {
        const auto x = double(_x);
        lnx.push_back(std::log(x));
        lnx2.push_back(std::pow(std::log(x), 2));
    }

    for (const auto _y: ys) {
        const auto y = double(_y);
        lny.push_back(std::log(y));
        lny2.push_back(std::pow(std::log(y), 2));
    }

    assert(lnx.size() == lny.size()); // not really necessary, but its gone at compile time

    for (size_t i = 0; i < lnx.size(); ++i) {
        lnxlny.push_back(lnx[i] * lny[i]);
    }

    double slnx = 0.0, slny = 0.0, slnxlny = 0.0, slnx2 = 0.0, slny2 = 0.0;
    for (int i = 0; i < lnx.size(); ++i) {
        slnx    += lnx[i];      slny  += lny[i];
        slnx2   += lnx2[i];     slny2 += lny2[i];
        slnxlny += lnxlny[i];
    }

    const double B = (N * slnxlny - slnx * slny) / (N * slnx2 - std::pow(slnx, 2));
    const double a = (slny - B * slnx) / N;
    const double A = std::exp(a);

    auto inner = [A, B](const size_t r) -> double {
        return A * std::pow(double(r), B);
    };

    double total = 0.0;
    for (const auto x : xs) {
        total += inner(x);
    }

    return [total, inner](const size_t r) -> double {
        return (1.0 / total) * inner(r);
    };

}

    
}

#endif
