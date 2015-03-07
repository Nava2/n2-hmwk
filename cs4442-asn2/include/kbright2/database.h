#ifndef __DATABASE_H
#define __DATABASE_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cstdarg>
#include <limits>

#include "utils.h"
#include "VectorHash.h"
#include "fileRead.h"

namespace kbright2 {
    
class DatabaseFactory; // forward

template <typename T>
class Database {

public:
    
    Database(const Database<T>&& other)
        : _n(other._n),
          _db(std::move(other._db)),
          _ngrams(std::move(other._ngrams)),
          _ngramsPerLevel(std::move(other._ngramsPerLevel)),
          _allTokens(std::move(other._allTokens)), 
          _maxCount(other._maxCount) {
        
    }
        
    Database<T> &operator=(const Database<T>&& other) {
        this->_n = other._n;
        this->_db = std::move(other._db);
        this->_allTokens = std::move(other._allTokens);
        this->_ngrams = std::move(other._ngrams);
        this->_ngramsPerLevel = std::move(other._ngramsPerLevel);
        this->_maxCount = other._maxCount;
        
        return *this;
    }

    
    ~Database() noexcept { }

    const std::size_t maxCount() const noexcept {
        return _maxCount;
    }

    const size_t count(const std::vector<T>& ngram) const {
        const auto it = _db.find(ngram);
        if (it != _db.end()) {
            return it->second.count;
        } else {
            return 0;
        }
    }
    
    const double prob(const std::vector<T> &ngram) const {
        const auto it = _db.find(ngram);
        if (it != _db.end()) {
            return it->second.prob;
        } else {
            return 0.0;
        }
    }
    
    const double nprob(const std::vector<T> &ngram) const {
        const auto it = _db.find(ngram);
        if (it != _db.end()) {
            return it->second.nprob;
        } else {
            return 0.0;
        }
    }

    const size_t ngramCount(const int size = -1) const noexcept {
        if (size < 1) {
            return _ngrams.size();
        } 
        
        if (size_t(size) >= _ngramsPerLevel.size()) {
            return 0;
        }
        
        return _ngramsPerLevel[size];
    }
    
    const std::vector<const std::vector<T>* > &ngrams() const noexcept {
        return _ngrams;
    }
    
    const std::vector<const std::vector<T>* > ngrams(const size_t size) const {
        if (size == 0) {
            return std::vector<const std::vector<T>*>(0);
        }
        
        if (size < 0 || size > _n) {
            throw std::out_of_range("Requested out of range ngram size");
        }
        
        std::vector<const std::vector<T>*> out;
        out.reserve(_allTokens.size());
        for (const auto& n: _ngrams) {
            if (n->size() == size) {
                out.push_back(n);
            }
        }
        
        out.shrink_to_fit();
        
        return out;
    }

    const std::vector<T>& tokens() const noexcept {
        return _allTokens;
    }
    
    const std::vector<std::vector<T> >& sentences() const noexcept {
        return _sentences;
    }

    const std::size_t n() const noexcept {
        return _n;
    }
    
    template <typename Iterable>
    const Database<T> merge(const Iterable& others) {
        std::unordered_map<std::vector<T>, std::size_t> out(_db);

        std::size_t mn = _n;
        for (const auto &other: others) {
            for (const auto &pair: other._db) {
                if (out.count(pair.first) == 0) {
                    out[pair.first] = {1, 0.0, 0.0};
                } else {
                    out[pair.first].count += pair.second.count;
                }
            }
            
            mn = std::max(mn, other.n());
        }
        
        return Database<T>(mn, std::move(out)); 
    }

    /**
     * @brief intersect Performs an intersection of `this` with `other`.
     * @param other
     * @return
     */
    const Database<T> intersect(const Database<T> &other) const {
        std::unordered_map<std::vector<T>, DataValue> out;

        for (const auto &pair: other._db) {
            if (_db.count(pair.first) >= 1) {
                out[pair.first] = { pair.second.count + _db[pair.first].count, 0.0, 0.0 };
            }
        }

        return Database<T>(_n, std::move(out));
    }

    /**
     * @brief notIn returns a Database instance with all of the ngrams in `this` that are not in `other`.
     * @param other
     * @return
     */
    const Database<T> notIn(const Database<T> &other) const {
        std::unordered_map<std::vector<T>, DataValue> out;

        for (const auto& pair: _db) {
            if (other._db.count(pair.first) == 0) {
                out[pair.first] = pair.second;
            }
        }

        return Database<T>(_n, std::move(out));
    }

private:
    
    typedef struct _DataValue {
        size_t count;
        double prob;
        double nprob;
    } DataValue;

    Database(const std::size_t N, const std::vector<T> &tokens)
        : _n(N),
          _allTokens(tokens) {
        intialize();
        initPostDb();
    }

    Database(const std::size_t N, const std::unordered_map<std::vector<T>, DataValue>&& copy)
        : _n(N),
          _db(copy) {
        initPostDb();
    }
    
    Database<T> &operator=(const Database<T>& other) = delete;
    
    void intialize() {

        using namespace std;
        
        if (_allTokens.size() < _n) {
            throw std::out_of_range("Tokens read < minimum ngram size.");
        }
        
        unordered_map<vector<T>, DataValue> tdb;
        
        // intitialize the "database" variables and increment coutns as they're found
        size_t count = 0;
        for (size_t i = 0; i < _allTokens.size(); ++i) {
            for (size_t j = 1; j <= _n && (i + j - 1 < _allTokens.size()); ++j) {
                std::vector<T> nGram(j);   // for temporarily storing tokens to go into next n-gram

                // Take next n tokens read from the input file
                for ( size_t k = 0; k < j; ++k ) {
                    nGram[k] = _allTokens[i + k]; // put next n tokens into vector temp
                }

                if ( tdb.count(nGram) == 0 ) { // nGram is not in the database yet, insert it with count 1
                    tdb[nGram] = DataValue{ 1, 0.0, 0.0 };
                } else { // nGram is already in the database, increase its count by 1
                    tdb[nGram].count += 1;
                }
                
                ++count;
            }
            
        }
        
        _db = std::move(tdb);
    }
    
    void initPostDb() {
        size_t count = 0;
        auto maxCount = std::numeric_limits<std::size_t>::min();
        
        std::vector<const std::vector<T> *> ngrams;
        ngrams.reserve(_db.size());
        std::vector<size_t> ngramCountPerLevel(_n + 1, 0);
        
        for (const auto& p: _db) {
            count += p.second.count;
            
            maxCount = std::max(maxCount, p.second.count);
            ngrams.push_back(&p.first);
            
            ngramCountPerLevel[p.first.size()] += 1;
        }

        ngrams.shrink_to_fit();
        
        // calculate the probability of occurance
        for (auto& p : _db) {
            p.second.prob = p.second.count / count; 
        }
        
        
        // compute the dependant probabilities
        for (auto& p : _db) {
            if (p.first.size() == 1) { // no such thing as a 0-gram..
                p.second.nprob = p.second.prob;
            } else {
                p.second.nprob = p.second.prob / _db[std::vector<T>(p.first.begin() + 1, p.first.end())].prob;
            }
        }
        
        _ngrams = std::move(ngrams);
        _ngramsPerLevel = std::move(ngramCountPerLevel);
        _sentences = parseSentences(_allTokens);
        _maxCount = maxCount;
    }

    const std::size_t _n;

    std::unordered_map<std::vector<T>, DataValue> _db;
    std::vector<const std::vector<T> *> _ngrams;
    std::vector<size_t> _ngramsPerLevel;
    std::vector<T> _allTokens;
    std::vector<std::vector<T>> _sentences;
    std::size_t _maxCount;
    
    friend DatabaseFactory;
};

/**
 * Build Database instances.
 */
class DatabaseFactory {

public:
    
    template <typename U>
    static Database<U> createFromFile(const size_t N, const std::string &path, const bool readEOS = false) {
        std::vector<U> tokens;
        read_tokens(path, tokens, readEOS);
        tokens.shrink_to_fit();

        return Database<U>(N, tokens);
    }

    template <typename U>
    static Database<U> create(const size_t N, const std::unordered_map<std::vector<U>, std::size_t> &copy) {
        return Database<U>(N, copy);
    }

    template <typename U>
    static Database<U> createMultiGram(const size_t N, const std::string &path, const bool readEOS = false) {
        std::vector<U> tokens;
        read_tokens(path, tokens, readEOS);
        
        std::vector<Database<U>> dbs { N - 2 };
        auto iter = dbs.first();
        for (size_t i = 2; i < N; ++i, ++iter) {
            dbs.emplace(iter, Database<U>(i, tokens)); 
        }
        
        const Database<U> that(1, tokens);

        return that.merge(dbs);
    }
};

}


#endif // __DATABASE_H
