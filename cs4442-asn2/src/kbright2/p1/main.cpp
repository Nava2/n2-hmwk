
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <VectorHash.h>

#include "kbright2/database.h"

/**
 * @brief The CLIInput struct holds information parsed from the command line
 */
typedef struct _CLIInput {
    const std::string first;
    const std::string second;
    const size_t n;
    const bool shouldPrint;


} CLIInput;

std::ostream& operator<<(std::ostream& os, const CLIInput& obj)
{
  // write obj to stream
    os << "CLIInput{first=" << obj.first
       << ", second=" << obj.second
       << ", n=" << obj.n
       << ", shouldPrint=" << std::boolalpha << obj.shouldPrint << "}";

  return os;
}

template <typename T>
const std::pair<std::size_t, double> computeDifference(const kbright2::Database<T>& first,
                                                       const kbright2::Database<T>& second,
                                                       const bool print) {

    const kbright2::Database<T> inter = second.notIn(first);

    if (print) {
        for (const auto &pair: inter.db()) {
            for (const auto & v: pair.first )
                std::cout << v;
            std::cout << std::endl;
        }
    }

    return std::pair<std::size_t, double>(inter.db().size(), 1.0 * inter.db().size() / second.db().size());
}

int main(const int argc, const char** argv) {

    if (argc != 5) {
        std::cerr << "Invalid arguments passed. " << std::endl;
        std::cerr << "Usage: cs4442b-asn2-p1 <firstInputText:string> <secondInputText:string> <nGramCount:int> <print:bool>" << std::endl;

        return 1;
    }

    const CLIInput input = { argv[1], argv[2], std::stoul(argv[3]), std::stoul(argv[4]) > 0 };

    std::cout << input << std::endl;

    const kbright2::Database<std::string> first(input.n, input.first), second(input.n, input.second);

    std::pair<std::size_t, double> difference = computeDifference(first, second, input.shouldPrint);

    std::cout << "Found " << difference.first << " (" << difference.second << "%) "
                  << "ngrams in second but not first." << std::endl;

    return 0;
}
