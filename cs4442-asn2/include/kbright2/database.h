#ifndef __DATABASE_H
#define __DATABASE_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <limits>
#include <functional>

#include "ngram.h"
#include "utils.h"
#include "VectorHash.h"
#include "fileRead.h"

namespace kbright2 {
    
static const std::string EOS_TERM = "<END>";
    
class DatabaseFactory; // forward

template <typename T>
class Database {

public: 

    typedef struct {
        size_t count;
        double prob;
        double depProb;
    } DataValue;


    using DataMap = std::unordered_map<NGram<T>, DataValue>;

    typedef std::function<double(const DataMap&, const std::unordered_set<T>&, const NGram<T>&, const size_t, const size_t)> ProbFunc;
    typedef ProbFunc DependantProbFunc;

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

    const size_t count(const NGram<T>& ngram) const {
        const auto it = _db.find(ngram);
        if (it != _db.end()) {
            return it->second.count;
        } else {
            return 0;
        }
    }
    
    const double prob(const NGram<T> &ngram) const {
        const auto it = _db.find(ngram);
        if (it != _db.end()) {
            return it->second.prob;
        } else {
            return 0.0;
        }
    }
    
    const double depProb(const NGram<T> &ngram) const {
        const auto it = _db.find(ngram);
        if (it != _db.end()) {
            return it->second.depProb;
        } else {
            return 0.0;
        }
    }

    const double sentenceDepProb(const std::vector<T> &sentence) {
        double out = 0.0;

        std::unique_ptr<NGram<T>> prev = nullptr;
        // initialize the first N
        const auto it = sentence.begin();
        for (size_t i = 1; i <= n() && it != sentence.end(); ++it, ++i) {

            NGram<T> *n;
            if (prev) {
                n = new NGram<T>(prev->asContext(*it));
            } else {
                n = new NGram<T>({ *it });
            }
            prev = std::unique_ptr<NGram<T>>(n);

            out += std::log(_db[n].depProb);
        }

        return out;
    }
    
    const std::vector<double> getNProbs(const std::vector<const NGram<T>* >& pngrams) const noexcept {
        std::vector<NGram<std::string> > ngrams;
        ngrams.reserve(pngrams.size());
        
        for (const auto& p: pngrams) {
            ngrams.push_back(*p);
        }
        
        return this->getNProbs(ngrams);
    }
    
    const std::vector<double> getNProbs(const std::vector<NGram<T> > &ngrams) const noexcept {
        std::vector<double> out;
        out.reserve(ngrams.size());
        
        for (const auto& ngram: ngrams) {
            out.push_back(this->depProb(ngram));
        }
        
        return out;
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
    
    const std::vector<const NGram<T>* > &ngrams() const noexcept {
        return _ngrams;
    }
    
    const std::vector<const NGram<T>* > ngrams(const size_t size) const {
        if (size == 0) {
            return std::vector<const NGram<T>*>(0);
        }
        
        if (size < 0 || size > _n) {
            throw std::out_of_range("Requested out of range ngram size");
        }
        
        std::vector<const NGram<T>*> out;
        out.reserve(_allTokens.size());
        for (const auto& n: _ngrams) {
            if (n->n() == size) {
                out.push_back(n);
            }
        }
        
        out.shrink_to_fit();
        
        return out;
    }

    const NGram<T>& tokens() const noexcept {
        return _allTokens;
    }
    
    const std::vector<NGram<T> >& sentences() const noexcept {
        return _sentences;
    }

    const std::size_t n() const noexcept {
        return _n;
    }
    
    /**
     * Map the internal data to a new map of type `U`. The function is applied on each element individually.
     *
     * @param mfn Function with arguments: `*this`, current NGram, and the count of occurances (this->count(currNgram))
     * @return new map of data.
     */
    template <typename U>
    const std::unordered_map<const NGram<T>*, U> map(const std::function<U(const Database<T> &, const NGram<T> &, const size_t)> &mfn) const {
        std::unordered_map<const NGram<T>*, U> out;

        for (const auto &p: _db) {
            out[&(p.first)] = mfn(*this, p.first, p.second.count);
        }

        return out;
    }

    template <typename Iterable>
    const Database<T> merge(const Iterable& others) {
        std::unordered_map<NGram<T>, std::size_t> out(_db);

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
        std::unordered_map<NGram<T>, DataValue> out;

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
        std::unordered_map<NGram<T>, DataValue> out;

        for (const auto& pair: _db) {
            if (other._db.count(pair.first) == 0) {
                out[pair.first] = pair.second;
            }
        }

        return Database<T>(_n, std::move(out), _probFn, _depProbFn);
    }

private:
    


    Database(const std::size_t N, const std::vector<std::string>& tokens,
             Database<T>::ProbFunc probFn,
             Database<T>::DependantProbFunc depProbFn)
        : _n(N),
          _probFn(probFn),
          _depProbFn(depProbFn),
          _allTokens(tokens) {
        intialize();
        initPostDb(probFn, depProbFn);
    }

    Database(const std::size_t N, const std::unordered_map<NGram<T>, DataValue>&& copy,
             Database<T>::ProbFunc probFn,
             Database<T>::DependantProbFunc depProbFn)
        : _n(N),
          _probFn(probFn),
          _depProbFn(depProbFn),
          _db(copy) {
        initPostDb(probFn, depProbFn);
    }
    
    Database<T> &operator=(const Database<T>& other) = delete;
    
    void intialize() {

        using namespace std;
        
        if (_allTokens.size() < _n) {
            throw std::out_of_range("Tokens read < minimum ngram size.");
        }
        
        unordered_map<NGram<T>, DataValue> tdb;
        
        // intitialize the "database" variables and increment coutns as they're found
        size_t count = 0;
        for (size_t i = 0; i < _allTokens.size(); ++i) {
            for (size_t j = 1; j <= _n && (i + j - 1 < _allTokens.size()); ++j) {
                vector<T> buff(j);   // for temporarily storing tokens to go into next n-gram

                // Take next n tokens read from the input file
                for ( size_t k = 0; k < j; ++k ) {
                    buff[k] = _allTokens[i + k]; // put next n tokens into vector temp
                }

                NGram<T> nGram(buff);
                if ( tdb.count(nGram) == 0 ) { // nGram is not in the database yet, insert it with count 1
                    tdb[nGram] = DataValue{ 1, 0.0, 0.0 };
                } else { // nGram is already in the database, increase its count by 1
                    tdb[nGram].count += 1;
                }
                
                ++count;
            }

            _vocab.insert(_allTokens[i]);
        }
        
        _db = std::move(tdb);
    }
    
    void initPostDb(typename Database<T>::ProbFunc probFn, typename Database<T>::DependantProbFunc depProbFn) {
        size_t count = 0;
        auto maxCount = std::numeric_limits<std::size_t>::min();
        
        std::vector<const NGram<T> *> ngrams;
        ngrams.reserve(_db.size());
        std::vector<size_t> ngramCountPerLevel(_n + 1, 0);
        
        for (const auto& p: _db) {
            count += p.second.count;
            
            maxCount = std::max(maxCount, p.second.count);
            ngrams.push_back(&p.first);
            
            ngramCountPerLevel[p.first.n()] += p.second.count;
        }

        ngrams.shrink_to_fit();
        
        // calculate the probability of occurance
        for (auto& p : _db) {
            p.second.prob = probFn(_db, _vocab, p.first, p.second.count, ngramCountPerLevel[p.first.n()]);
        }
        
        
        // compute the dependant probabilities
        for (auto& p : _db) {
            p.second.depProb = depProbFn(_db, _vocab, p.first, p.second.count, ngramCountPerLevel[p.first.n()]);
        }
        
        _ngrams = std::move(ngrams);
        _ngramsPerLevel = std::move(ngramCountPerLevel);
        _sentences = parseSentences(_allTokens);
        _maxCount = maxCount;
    }

    const size_t _n;
    const ProbFunc _probFn;
    const DependantProbFunc _depProbFn;

    std::unordered_map<NGram<T>, DataValue> _db;
    std::vector<const NGram<T> *> _ngrams;
    std::vector<size_t> _ngramsPerLevel;
    std::vector<T> _allTokens;
    std::unordered_set<T> _vocab;
    std::vector<std::vector<T>> _sentences;
    size_t _maxCount;


    
    friend class DatabaseFactory;
};

class NGramProbFunc {
public:
    template <typename T>
    static double simpleProbability(const typename Database<T>::DataMap&, const std::unordered_set<T>&,
                                    const NGram<T>&, // unused params
                                    size_t count, const size_t ncount) {
        return (1.0 * count) / ncount;
    }

    template <typename T>
    static double mle(const typename Database<T>::DataMap& otherInfo, const std::unordered_set<T>&,
                      const NGram<T>& ngram, const size_t count, const size_t ncount) {
        if (ngram.n() == 1) { // no such thing as a 0-gram..
            const auto it = otherInfo.find(ngram);
            if (it != otherInfo.end()) {
                return it->second.prob;
            } else {
                throw std::invalid_argument("ngram is not found in otherInfo");
            }
        } else {
            const auto ctxtIt = otherInfo.find(ngram.contextNgram());
            if (ctxtIt != otherInfo.end()) {
                return (1.0 * count) / ctxtIt->second.count;
            } else {
                throw std::invalid_argument("ngram's context is invalid in otherInfo");
            }

        }
    }

    template <typename T>
    static double dependantProb(const typename Database<T>::DataMap& other, const std::unordered_set<T>&,
                                const NGram<T>& that, const size_t, const size_t) {
         const auto tit = other.find(that);
         if (tit == other.end()) {
             throw std::invalid_argument("Could not find 'that' ngram's data");
         }

         if (that.n() > 1) {
             const auto oit = other.find(that.contextNgram());
             if (oit != other.end()) {
                 // it wasn't a zero ngram
                 return tit->second.prob / oit->second.prob;
             }

         }

         return tit->second.prob;
     }


};

/**
 * Build Database instances.
 */
class DatabaseFactory {

public:
    
    template <typename U>
    static Database<U> createFromFile(const size_t N, const std::string &path, const bool readEOS = false,
                                      typename Database<U>::ProbFunc probFn = &NGramProbFunc::simpleProbability<U>,
                                      typename Database<U>::DependantProbFunc depProbFn = &NGramProbFunc::mle<U>) {
        std::vector<U> tokens;
        read_tokens(path, tokens, readEOS);
        tokens.shrink_to_fit();

        return DatabaseFactory::createFromTokens<U>(N, tokens, probFn, depProbFn);
    }
    
    template <typename U>
    static Database<U> createFromTokens(const size_t N, const std::vector<U>& tokens,
                                        typename Database<U>::ProbFunc probFn = &NGramProbFunc::simpleProbability<U>,
                                        typename Database<U>::DependantProbFunc depProbFn = &NGramProbFunc::mle<U>) {
        return Database<U>(N, tokens, probFn, depProbFn);
    }

    template <typename U>
    static Database<U> create(const size_t N, const std::unordered_map<std::vector<U>, std::size_t> &copy,
                              typename Database<U>::ProbFunc probFn = &NGramProbFunc::simpleProbability<U>,
                              typename Database<U>::DependantProbFunc depProbFn = &NGramProbFunc::mle<U>) {
        return Database<U>(N, copy, probFn, depProbFn);
    }
};

} // end kbright2 namespace


#endif // __DATABASE_H

