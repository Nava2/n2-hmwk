
#include "catch/catch.hpp"

#include "kbright2/database.h"

#include <string>

using namespace std;
using namespace kbright2;

#define NGRAM_PTR(...) std::unique_ptr<NGram<string>>(new NGram<string>(__VA_ARGS__))

namespace data {
    
const vector<string> train = {
    "xx", "ab", "ab", EOS_TERM,
    "bc", "bc", "bc", EOS_TERM
};

const vector<string> test = {
    "ab",
    EOS_TERM,
    "bc",
    EOS_TERM,
    "xx",
    EOS_TERM
};

}

#define DBL_EQ(a, b) int((a) * 10000) == int((b) * 10000)

SCENARIO( "Create Databases", "[kbright2::Database]" ) {
    
    
    const NGram<string> nbcbc {"bc", "bc"}, neos{EOS_TERM};
    
    GIVEN( "Valid tokens" ) {
        Database<string> db = DatabaseFactory::createFromTokens(3, data::train);
        
        REQUIRE(db.maxCount() == 3);
        REQUIRE(db.count(nbcbc) == 2);
        REQUIRE(DBL_EQ(db.prob(nbcbc), 0.2857));
        REQUIRE(DBL_EQ(db.nprob(nbcbc), 0.6666));  

        const auto nprobs = db.getNProbs(vector<NGram<string>>({nbcbc, neos}));
        REQUIRE(DBL_EQ(nprobs[0], 0.6666));
        REQUIRE(DBL_EQ(nprobs[1], 0.2500));
        
        REQUIRE(db.ngramCount() == 16);
        REQUIRE(db.ngramCount(0) == 16); // all engrams
        REQUIRE(db.ngramCount(1) == 8);
        REQUIRE(db.ngramCount(2) == 7);
        REQUIRE(db.ngramCount(3) == 6);
        
        const auto &punis = db.ngrams(1);
        vector<NGram<string> > unis;
        unis.reserve(punis.size());
        for (const auto p : punis) 
            unis.push_back(*p);
        
        // the order of NGrams is NOT guarenteed, so search for them
        const std::vector<NGram<string> > expected({{"xx"}, {"ab"}, {EOS_TERM}, {"bc"}});
        for (const auto& ng : expected)
            REQUIRE(find(unis.begin(), unis.end(), ng) != unis.end());
        
    }
}