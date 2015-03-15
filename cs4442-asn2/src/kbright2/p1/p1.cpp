
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <VectorHash.h>

#include "kbright2/database.h"

using namespace kbright2;

/**
 * Computes the difference between two databases, returning the count and percentage as `size_t` and `double`
 * respectively.
 * @param first First database
 * @param second Second database, checks if values in second are not in first
 * @param print If true will print all non-matches, otherwise no effect.
 *
 * @return std::pair with the count of differences in std::pair::first and the percentage of total in std::pair::second
 */
template <typename T>
const std::pair<std::size_t, double> computeDifference(const Database<T>& first,
                                                       const Database<T>& second,
                                                       const size_t n, 
                                                       const bool print) {

    const auto notIn = second.notIn(first);

    const std::pair<std::size_t, double> out(notIn.ngramCount(n), 1.0 * notIn.ngramCount(n) / second.ngramCount(n));

    std::cout << std::setprecision(3) << (100.0 * out.second) << std::endl;
    if (print) {
        const auto inter = first.intersect(second);
        for (const auto& ngram: inter.ngrams(n)) {
            std::cout << ngram->toString() << std::endl;
        }
    }

    return out;
}

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


int main(const int argc, const char** argv) {

    if (argc != 5) {
        std::cerr << "Invalid arguments passed. " << std::endl;
        std::cerr << "Usage: P1 <firstInputText:string> <secondInputText:string> <nGramCount:int> <print:bool>" << std::endl;

        return 1;
    }

    const CLIInput input = { argv[1], argv[2], std::stoul(argv[3]), std::stoul(argv[4]) > 0 };

//     std::cout << input << std::endl;

    const auto first = DatabaseFactory::createFromFile<std::string>(input.n, input.first), 
            second = DatabaseFactory::createFromFile<std::string>(input.n, input.second);

    computeDifference(first, second, input.n, input.shouldPrint);

    return 0;
}
