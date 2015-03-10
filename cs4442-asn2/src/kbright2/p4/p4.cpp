
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
#include <memory>

#include "utilsToStudents.h"
#include "fileRead.h"

#include "kbright2/database.h"


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
    const std::string model;
    const std::string sentences;
    const size_t n;
    const double lmparam;
    const LangModel langModel;
} CLIInput;

std::ostream& operator<<(std::ostream& os, const CLIInput& obj)
{
    // write obj to stream
    os << "CLIInput{model=" << obj.model
       << ", sentences=" << obj.sentences
       << ", langModel=" << (obj.langModel == LangModel::GOOD_TURING ? "Good-Turing" : "Add-Delta")
       << ", lmparam=" << std::setprecision(5) << obj.lmparam
       << ", n=" << obj.n << "}";

    return os;
}

int main(const int argc, const char** argv) {

    if (argc != 6) {
        std::cerr << "Invalid arguments passed. " << std::endl;
        std::cerr << "Usage: P4 <text:path> <sentences:path> <nGramCount:int> <language-param:float> <language-model:0|1>" << std::endl;

        return 1;
    }

    srand(7777);

    const CLIInput input = { argv[1], argv[2], std::stoul(argv[3]), std::stod(argv[4]), LangModel(std::stoul(argv[5])) };

    std::cout << input << std::endl;

//     const auto model = DatabaseFactory::createFromFile<std::string>(input.n, input.model, true,
//         NGramProbFunc::delta_add<std::string>(input.lmparam), &NGramProbFunc::dependantProb<std::string>);

    const auto model = DatabaseFactory::createFromFile<std::string>(input.n, input.model, true,
        &NGramProbFunc::good_turing, &NGramProbFunc::dependantProb<std::string>);
    
    vector<std::string> stokens;
    read_tokens(input.sentences, stokens, true);

    const auto sentences = parseSentences(stokens);
    for (const auto& sentence: sentences) {
        cout << setprecision(5) << model.sentenceDepProb(sentence) << endl;
    }

    return 0;
}

