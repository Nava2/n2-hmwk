#ifndef __UTILS_H
#define __UTILS_H

#include <string>
#include <vector>
#include <unordered_map>
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

    
}

#endif
