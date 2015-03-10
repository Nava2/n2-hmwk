
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
#include <memory>

#include "utilsToStudents.h"
#include "fileRead.h"

#include "kbright2/database.h"
#include "kbright2/smoothing/good_turing.h"
#include "kbright2/smoothing/add_delta.h"


using namespace kbright2;
using namespace std;

enum class LangModel: size_t {
    GOOD_TURING = 0,
    ADD_DELTA = 1
};

/**
 * @brief The CLIInput struct holds information parsed from the command line
 */
typedef struct {
    const size_t senLength;
    const size_t n;
    const double delta;
} CLIInput;

std::ostream& operator<<(std::ostream& os, const CLIInput& obj)
{
    // write obj to stream
    os << "CLIInput{senLength=" << obj.senLength
       << ", delta=" << std::setprecision(5) << obj.delta
       << ", n=" << obj.n << "}";

    return os;
}

int main(const int argc, const char** argv) {

    if (argc != 4) {
        std::cerr << "Invalid arguments passed. " << std::endl;
        std::cerr << "Usage: P5 <nGramCount:int> <delta:float> <senLength:int>" << std::endl;

        return 1;
    }

    srand(7777);

    const CLIInput input = { std::stoul(argv[1]), std::stoul(argv[2]), std::stod(argv[3]) };

    std::cout << input << std::endl;


    std::vector<char> tokens;
    read_tokens("test.txt", tokens, false);
    tokens.shrink_to_fit();

    std::vector<std::vector<char> > sentences;
    sentences.reserve(tokens.size() / input.senLength + 1);

//    cout << std::distance(tokens.begin(), tokens.end()) << " wat" << endl;
    for (auto cit = tokens.begin(); cit != tokens.end() && (std::distance(cit, tokens.end()) > input.senLength) ; cit += input.senLength) {
        sentences.push_back(std::vector<char>(cit, cit + input.senLength));
    }

    return 0;
}

