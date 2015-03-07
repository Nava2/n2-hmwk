
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <VectorHash.h>

#include "kbright2/database.h"

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
const std::pair<std::size_t, double> computeDifference(const kbright2::Database<T>& first,
                                                       const kbright2::Database<T>& second,
                                                       const bool print) {

    const kbright2::Database<T> inter = second.notIn(first);

    const std::pair<std::size_t, double> out(inter.db().size(), 1.0 * inter.db().size() / second.db().size());

    std::cout << std::setprecision(3) << out.second << std::endl;
    if (print) {
        for (const auto &pair: inter.db()) {
            for (const auto & v: pair.first )
                std::cout << v;
            std::cout << std::endl;
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
        std::cerr << "Usage: cs4442b-asn2-p1 <firstInputText:string> <secondInputText:string> <nGramCount:int> <print:bool>" << std::endl;

        return 1;
    }

    const CLIInput input = { argv[1], argv[2], std::stoul(argv[3]), std::stoul(argv[4]) > 0 };

    std::cout << input << std::endl;

    const kbright2::Database<std::string> first(input.n, input.first), second(input.n, input.second);

    computeDifference(first, second, input.shouldPrint);

    return 0;
}
