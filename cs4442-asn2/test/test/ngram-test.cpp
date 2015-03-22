
#include "catch/catch.hpp"

#include "kbright2/ngram.h"

#include <string>

using namespace std;
using namespace kbright2;

#define NGRAM_PTR(...) std::unique_ptr<NGram<string>>(new NGram<string>(__VA_ARGS__))

SCENARIO( "Create NGrams", "[kbright2::NGram]" ) {
    
    GIVEN( "Valid multigram" ) {
        std::unique_ptr<NGram<string>> ng;
        
        WHEN( "built with initializer list" ) {
            ng = NGRAM_PTR({"c0", "c1", "v"});
        }
        
        WHEN( "built with iterators" ) {
            const vector<string> cont{"c0", "c1", "v"};
            ng = NGRAM_PTR(cont.end() - 1, cont.begin(), cont.end() - 1);
        }
        
        WHEN( "built with vector" ) {
            const vector<string> cont{"c0", "c1", "v"};
            ng = NGRAM_PTR(cont);
        }
        
        WHEN( "built with values" ) {
            ng = NGRAM_PTR("v", std::vector<string>({"c0", "c1"}));
        }
        
        WHEN( "built with hybrid" ) {
            ng = NGRAM_PTR("v", {"c0", "c1"});
        }
        
        REQUIRE(ng->value() == "v");
        REQUIRE(ng->context() == vector<string>({"c0", "c1"}));
        REQUIRE(ng->n() == 3);
    }
    
    
    GIVEN( "Valid unigram" ) {
        std::unique_ptr<NGram<string>> ng;
        
        WHEN( "built with initializer list" ) {
            ng = NGRAM_PTR({"v"});
        }
        
        REQUIRE(ng);
        REQUIRE(ng->value() == "v");
        REQUIRE(ng->n() == 1);
        REQUIRE(ng->context().size() == 0);
    }
}

SCENARIO( "Get a list of NGrams from a context set", "[kbright2::NGram]") {
    GIVEN( "No context" ) {
        const vector<NGram<string>> ng = NGram<string>::createCombinations({"a", "b"}, std::vector<string>());
        
        REQUIRE(ng == vector<NGram<string> >({ NGram<string>{"a"}, NGram<string>{"b"} }) );
    }
    
    GIVEN( "Simple context" ) {
        const vector<NGram<string>> ng = NGram<string>::createCombinations({"a", "b"}, {"c"});
        
        REQUIRE(ng == vector<NGram<string> >({ NGram<string>{"c", "a"}, NGram<string>{"c", "b"} }) );
    }
}
