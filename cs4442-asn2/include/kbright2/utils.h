#ifndef __UTILS_H
#define __UTILS_H

#include <string>
#include <vector>
#include <unordered_set>
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
    
}

#endif