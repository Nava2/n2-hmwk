
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
        REQUIRE(ng->contextNgram() == NGram<string>({"c0", "c1"}));
        REQUIRE(ng->asContext("v2") == NGram<string>({"c1", "v", "v2"}));
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
        REQUIRE_THROWS_AS(ng->contextNgram(), std::invalid_argument); 
        REQUIRE(ng->asContext("v2") == NGram<string>({"v2"}));
    }
    
    WHEN("building a context NGram") {
        
        std::unique_ptr<NGram<string>> ng;
        std::unique_ptr<NGram<string>> ctx;
        size_t newN;
        
        GIVEN( "unigram") {
            ng = NGRAM_PTR({"v"});
            
            GIVEN("newN = 1") {
                newN = 1;
                ctx = NGRAM_PTR(ng->asContext("v2", 1));
                REQUIRE(ctx->value() == "v2");
                REQUIRE(ctx->context() == vector<string>());
            }
            
            GIVEN("newN = 2") {
                newN = 2;
                ctx = NGRAM_PTR(ng->asContext("v2", 2));
                REQUIRE(ctx->value() == "v2");
                REQUIRE(ctx->context() == vector<string>({"v"}));
            }  
        }
        
        GIVEN( "trigram") {
            ng = NGRAM_PTR({"c0", "c1", "v"});
            
            GIVEN("newN = 4") {
                newN = 4;
                ctx = NGRAM_PTR(ng->asContext("v2", newN));
                REQUIRE(ctx->value() == "v2");
                REQUIRE(ctx->context() == vector<string>({"c0", "c1", "v"}));
            }
            
            GIVEN("newN = 3") {
                newN = 3;
                ctx = NGRAM_PTR(ng->asContext("v2", newN));
                REQUIRE(ctx->value() == "v2");
                REQUIRE(ctx->context() == vector<string>({"c1", "v"}));
            }
            
            GIVEN("newN = 2") {
                newN = 2;
                ctx = NGRAM_PTR(ng->asContext("v2", newN));
                REQUIRE(ctx->value() == "v2");
                REQUIRE(ctx->context() == vector<string>({"v"}));
            }
            
            GIVEN("newN = 1") {
                newN = 1;
                ctx = NGRAM_PTR(ng->asContext("v2", newN));
                REQUIRE(ctx->value() == "v2");
                REQUIRE(ctx->context() == vector<string>());
            }
        }
        
        REQUIRE(ctx->n() == newN);
        REQUIRE_THROWS_AS(ng->asContext("", 0), std::invalid_argument);
        REQUIRE_THROWS_AS(ng->asContext("", ng->n() + 2), std::invalid_argument);
    }
}

SCENARIO( "Get a list of NGrams from a context set", "[kbright2::NGram]") {
    GIVEN( "No context" ) {
        const vector<NGram<string>> ng = NGram<string>::createCombinations({"a", "b"}, std::vector<string>());
        
        REQUIRE(ng == vector<NGram<string> >({ NGram<string>{"a"}, NGram<string>{"b"} }) );
    }
    
    GIVEN( "Simple NGram" ) {
        const vector<NGram<string>> ng = NGram<string>::createCombinations({"a", "b"}, NGram<string>{"c"}, 2);
        
        REQUIRE(ng == vector<NGram<string> >({ NGram<string>{"c", "a"}, NGram<string>{"c", "b"} }) );
    }
    
    GIVEN( "Simple trigram" ) {
        const vector<NGram<string>> ng = NGram<string>::createCombinations({"a", "b"}, NGram<string>{"cc", "c", "v"}, 3);
        
        REQUIRE(ng == vector<NGram<string> >({ NGram<string>{"c", "v", "a"}, NGram<string>{"c", "v", "b"} }) );
    }
}
