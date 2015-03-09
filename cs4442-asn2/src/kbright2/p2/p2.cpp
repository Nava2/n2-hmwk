
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>

#include "kbright2/database.h"


using namespace kbright2;

/**
 * @brief The CLIInput struct holds information parsed from the command line
 */
typedef struct _CLIInput {
    const std::string train;
    const std::string test;
    const size_t n;
} CLIInput;

std::ostream& operator<<(std::ostream& os, const CLIInput& obj)
{
    // write obj to stream
    os << "CLIInput{train=" << obj.train
       << ", test=" << obj.test
       << ", n=" << obj.n << "}";

    return os;
}


int main(const int argc, const char** argv) {

    if (argc != 4) {
        std::cerr << "Invalid arguments passed. " << std::endl;
        std::cerr << "Usage: P2 <train:string> <test:string> <nGramCount:int>" << std::endl;

        return 1;
    }

    const CLIInput input = { argv[1], argv[2], std::stoul(argv[3]) };

    std::cout << input << std::endl;

    const auto train = DatabaseFactory::createFromFile<std::string>(input.n, input.train, true); 
    std::vector<std::string> tokens;
    read_tokens(input.test, tokens, true);
    
    const auto tests = kbright2::parseSentences(tokens);
    
    size_t zero_count = 0;
    for (const auto& sentence : tests) {
        if (sentence.size() < input.n) {
            continue;
        }
        
        double sentenceProb = 0.0;
        
        for (size_t i = 0; (i < (sentence.size() - input.n + 1)) && sentenceProb == 0.0; ++i) {
            const NGram<std::string> ngram(sentence.begin() + i + input.n - 1, sentence.begin() + i, sentence.begin() + i + input.n - 1);
            sentenceProb += train.depProb(ngram);
        }
        
        if (sentenceProb == 0.0) {
            ++zero_count;
        }
        
    }
    
    std::cout << std::setprecision(2) << (1.0* zero_count / tests.size()) << std::endl;

//     computeDifference(first, second, input.shouldPrint);

    return 0;
}

