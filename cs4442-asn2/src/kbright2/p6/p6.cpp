
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
typedef struct _CLIInput {
    const std::string train;
    const std::string check;
    const std::string dictionary;
    const size_t n;
    const size_t t;
    const double delta;
    const LangModel langModel;
} CLIInput;

std::ostream& operator<<(std::ostream& os, const CLIInput& obj)
{
    // write obj to stream
    os << "CLIInput{train=" << obj.train
       << ", check=" << obj.check
       << ", dictionary=" << obj.dictionary
       << ", langModel=" << (obj.langModel == LangModel::GOOD_TURING ? "Good-Turing" : "Add-Delta")
       << ", delta=" << std::setprecision(5) << obj.delta
       << ", t=" << obj.t
       << ", n=" << obj.n << "}";

    return os;
}

int main(const int argc, const char** argv) {

    if (argc != 8) {
        std::cerr << "Invalid arguments passed. " << std::endl;
        std::cerr << "Usage: P6 <trainText:path> <checkText:path> <dictionary:path> <nGramCount:int> <threshold:size_t> <delta:float> <language-model:0|1>" << std::endl;

        return 1;
    }

    srand(7777);

    const CLIInput input = { argv[1], argv[2], argv[3],
                             std::stoul(argv[4]),  // n
                             std::stoul(argv[5]),  // t
                             std::stod(argv[6]),   // delta
                             LangModel(std::stoul(argv[7])) };

    std::cout << input << std::endl;

    std::vector<std::string> trainTokens;
    read_tokens(input.train, trainTokens, true);

    const auto trainModel = input.langModel == LangModel::GOOD_TURING
            ? kbright2::good_turing::createModel<std::string>(input.n, trainTokens, input.t)
            : kbright2::add_delta::createModel<std::string>(input.n, trainTokens, input.delta);

    std::vector<std::string> dictWords;
    read_tokens(input.dictionary, dictWords, false);
    dictWords.shrink_to_fit();

    std::vector<std::string> checkTokens;
    read_tokens(input.check, checkTokens, true);

    const std::vector<std::vector<std::string> > checkSentences = kbright2::parseSentences(checkTokens);

    std::vector<std::vector<std::string> > likelySentences;
    likelySentences.reserve(checkSentences.size());

    for (const auto& check: checkSentences) {

        auto best = check;
        double bprob = trainModel.sentenceDepProb(check);

        for (size_t i = 0; i < check.size(); ++i) {
            std::vector<std::string> buff(check.begin(), check.end());

            const auto w2check = kbright2::getLikelyWords(dictWords, buff[i]);
            for (const auto& w2 : w2check) {
                buff[i] = w2;

                const double prob = trainModel.sentenceDepProb(buff);
                if (prob > bprob) {
                    best = buff;
                    bprob = prob;
                }
            }
        }

        likelySentences.push_back(best);
    }

    for (const auto& s: likelySentences) {
        for (const auto& w: s) {
            cout << w << " ";
        }
        cout << endl;
    }

    return 0;
}


