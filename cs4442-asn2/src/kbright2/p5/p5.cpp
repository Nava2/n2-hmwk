
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
#include <memory>
#include <array>

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
    const double delta;
    const size_t n;
    const size_t senLength;
} CLIInput;

std::ostream& operator<<(std::ostream& os, const CLIInput& obj)
{
    // write obj to stream
    os << "CLIInput{senLength=" << obj.senLength
       << ", delta=" << std::setprecision(5) << obj.delta
       << ", n=" << obj.n << "}";

    return os;
}

const std::array<std::string, 6> LANGS{ {"english", "french", "danish", "italian", "latin", "sweedish"} };

const bool READ_LATIN_ONLY = false;


int main(const int argc, const char** argv) {

    if (argc != 4) {
        std::cerr << "Invalid arguments passed. " << std::endl;
        std::cerr << "Usage: P5 <nGramCount:int> <delta:float> <senLength:int>" << std::endl;

        return 1;
    }

    srand(7777);

    const CLIInput input = { std::stod(argv[2]), std::stoul(argv[1]), std::stoul(argv[3]) };

    std::cout << input << std::endl;

    std::vector<Database<char> > trainModels;
    trainModels.reserve(LANGS.size());

    std::vector<std::vector<char>> langTokens;
    langTokens.reserve(LANGS.size());

    std::vector<std::vector<std::vector<char> > > langSentences;
    langSentences.reserve(LANGS.size());

    for (const auto& lang : LANGS) {
        std::vector<char> tokens;
        read_tokens(lang + "1.txt", tokens, READ_LATIN_ONLY);
        tokens.shrink_to_fit();
        langTokens.push_back(tokens);

        tokens.clear();
        read_tokens(lang + "2.txt", tokens, READ_LATIN_ONLY);
        tokens.shrink_to_fit();
        langSentences.push_back(kbright2::parseSentences(tokens, input.senLength));

        trainModels.push_back(kbright2::add_delta::createModel(input.n, tokens, input.delta));
    }

    std::vector<std::vector<size_t> > conf(LANGS.size(), std::vector<size_t>(LANGS.size(), 0));

    for (size_t i = 0; i < LANGS.size(); ++i) {
        for (const auto s : langSentences[i]) {
            size_t bestj = 0;
            double best = std::numeric_limits<double>::lowest();
            for (size_t j = 0; j < LANGS.size(); ++j) {
                const auto& model = trainModels[j];
                const double prob = model.sentenceDepProb(s);

                if (prob > best) {
                    best = prob;
                    bestj = j;
                }
            }

            conf[i][bestj] += 1;
        }
    }

    size_t total = 0;
    size_t bad = 0;
    for (size_t i = 0; i < LANGS.size(); ++i) {
        for (size_t j = 0; j < LANGS.size(); ++j) {
            const auto v = conf[i][j];
            total += v;
            if (i != j) {
                bad += v;
            }
        }
    }

    cout << setprecision(4) << (100.0 * bad) / total << endl;

    for (auto& a: conf) {
        for (auto v: a) {
            cout << setw(5) << v << ' ';
        }
        cout << endl;
    }

    return 0;
}

