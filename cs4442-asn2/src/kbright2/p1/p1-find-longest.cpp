
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <VectorHash.h>

#include "kbright2/database.h"

using namespace kbright2;

/**
 * @brief The CLIInput struct holds information parsed from the command line
 */
typedef struct _CLIInput {
    const std::string first;
    const std::string second;
    const size_t n;
} CLIInput;

std::ostream& operator<<(std::ostream& os, const CLIInput& obj)
{
    // write obj to stream
    os << "CLIInput{first=" << obj.first
       << ", second=" << obj.second
       << ", n=" << obj.n << "}";

    return os;
}


int main(const int argc, const char** argv) {

    if (argc != 4) {
        std::cerr << "Invalid arguments passed. " << std::endl;
        std::cerr << "Usage: P1-find-longest <firstInputText:string> "
            << "<secondInputText:string> <nGramCount:int>" << std::endl;

        return 1;
    }

    const CLIInput input = { argv[1], argv[2], std::stoul(argv[3]) };

//     std::cout << input << std::endl;

    const auto first = DatabaseFactory::createFromFile<std::string>(input.n, 
                                                                    input.first), 
            second = DatabaseFactory::createFromFile<std::string>(input.n, 
                                                                  input.second);

    const auto inter = first.intersect(second);
    // iterate over finding the intersection
            
    std::vector<const kbright2::NGram<std::string>*> largestNgrams;
    size_t n = input.n;
    for ( ; n > 0; --n) {
        const auto ngrams = inter.ngrams(n);
        if (ngrams.size() != 0) {
            largestNgrams = ngrams;
            break;
        }
    }

    std::cout << "N = " << n << std::endl;
    for (const auto& ngram: largestNgrams) {
        std::cout << ngram->toString() << std::endl;
    }
    
    if (largestNgrams.size() == 0) {
        std::cout << "No common n-grams." << std::endl;
    }
    
    return 0;
}
