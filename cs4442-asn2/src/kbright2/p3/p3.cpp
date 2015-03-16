
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
#include <memory>
#include <ctime>

#include "utilsToStudents.h"

#include "kbright2/database.h"


using namespace kbright2;
using namespace std;

/**
 * @brief The CLIInput struct holds information parsed from the command line
 */
typedef struct {
    const std::string text;
    const size_t n;
} CLIInput;

std::ostream& operator<<(std::ostream& os, const CLIInput& obj)
{
    // write obj to stream
    os << "CLIInput{train=" << obj.text
       << ", n=" << obj.n << "}";

    return os;
}

const vector<string> generateSentence(const Database<string> &db) {
    
    vector<string> sentence;
    
    vector<string> vocab;
    vocab.reserve(db.ngramCount(1));
    for (const auto &n : db.ngrams(1)) {
        vocab.push_back(n->value());
    }
    
    
    bool eosFound = false;
    std::vector<NGram<std::string> > ngramsBuff;
    ngramsBuff.reserve(vocab.size());
    std::vector<double> probsBuff;
    probsBuff.reserve(vocab.size());
    
    // initialize the first N
    for (size_t i = 1; i <= db.n() && !eosFound; ++i) {
        
        std::vector<std::string> context;
        context.reserve(std::min(int(sentence.size()), int(i) - 1));
        if (sentence.size() >= 1) {
            context.insert(context.begin(), sentence.end() - i + 1, sentence.end());
        }
        
        ngramsBuff = NGram<string>::createCombinations(vocab, context);
        probsBuff = db.getNProbs(ngramsBuff);
        
        const size_t index = drawIndex(probsBuff);
        const auto &word = ngramsBuff[index].value();
        sentence.push_back(word);
        
        eosFound = (word == EOS_TERM);
    }
    
    while (!eosFound) {
        std::vector<std::string> context; // build previous context
        context.reserve(db.n() - 1);
        if (db.n() > 1) {
            context.insert(context.begin(), sentence.end() - db.n() + 1, sentence.end());
        }
        
        ngramsBuff = NGram<string>::createCombinations(vocab, context);
        probsBuff = db.getNProbs(ngramsBuff);
        
        const size_t index = drawIndex(probsBuff);
        const auto &word = ngramsBuff[index].value();
        sentence.push_back(word);
        
        eosFound = (word == EOS_TERM);
    }
    
    return sentence;
}


int main(const int argc, const char** argv) {

    if (argc != 3) {
        std::cerr << "Invalid arguments passed. " << std::endl;
        std::cerr << "Usage: P3 <text:path> <nGramCount:int>" << std::endl;

        return 1;
    }

    srand(time(NULL));
    
    const CLIInput input = { argv[1], std::stoul(argv[2]) };

//     std::cout << input << std::endl;

    const auto train = DatabaseFactory::createFromFile<std::string>(input.n, input.text, true); 
    
    const auto sentence = generateSentence(train);
   
//     cout << "Generated Sentence:" << endl << "--> ";
    for (const auto &word: sentence) {
        cout << word << " ";
    }
    cout << endl;

    return 0;
}

