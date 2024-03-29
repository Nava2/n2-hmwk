#ifndef __DATABASE_H
#define __DATABASE_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <limits>
#include <functional>
#include <memory>
#include <cmath>
#include <map>
#include <algorithm>

#include "ngram.h"
#include "utils.h"
#include "VectorHash.h"
#include "fileRead.h"

namespace kbright2 {
    
static const std::string EOS_TERM = "<END>";
    
class DatabaseFactory; // forward

/**
 * @brief The Database class is an overarching collection of `NGram<T>` instances with useful methods to compute NLP 
 * calculations.
 * 
 * Building of Database instances is done via the DatabaseFactory class and all instances that exist should be `const`
 * and internally immutable. This was done intentionally to stop accidental copying of memory under performance 
 * concerns with datasets of large `N` values. Upon construction, probabilities are computed via lamda expressions or 
 * function pointers and stored for later usage. 
 * 
 * <strong>Note:</strong> This class is only valud for <code>T = char</code> or <code>T = std::string</code>. 
 */
template <typename T>
class Database {

public: 

    typedef struct {
        size_t count;
        double prob;
        double depProb;
    } DataValue;


    using DataMap = std::unordered_map<NGram<T>, DataValue>;

    typedef std::function<double(const Database<T>&, const NGram<T>&, const size_t)> ProbFunc;
    typedef ProbFunc DependantProbFunc;

    Database(const Database<T>&& other)
        : _n(other._n),
          _db(std::move(other._db)),
          _ngrams(std::move(other._ngrams)),
          _ngramsPerLevel(std::move(other._ngramsPerLevel)),
          _allTokens(std::move(other._allTokens)),
          _maxCount(other._maxCount), 
          _unfoundProb(other._unfoundProb),
          _unfoundDepProb(other._unfoundDepProb) {

    }

    Database(const Database<T>& other)
        : _n(other._n),
          _db(other._db),
          _ngrams(other._ngrams),
          _ngramsPerLevel(other._ngramsPerLevel),
          _allTokens(other._allTokens),
          _maxCount(other._maxCount),
          _unfoundProb(other._unfoundProb),
          _unfoundDepProb(other._unfoundDepProb)  {

    }
        
    Database<T> &operator=(const Database<T>&& other) {
        this->_n = other._n;
        this->_db = std::move(other._db);
        this->_allTokens = std::move(other._allTokens);
        this->_ngrams = std::move(other._ngrams);
        this->_ngramsPerLevel = std::move(other._ngramsPerLevel);
        this->_maxCount = other._maxCount;
        this->_unfoundProb = other._unfoundProb;
        this->_unfoundDepProb = other._unfoundDepProb;
        
        return *this;
    }

    
    ~Database() noexcept { }

    const Database<T> recalculate(ProbFunc probFn, DependantProbFunc depProbFn) const {
        return Database<T>(_n, this->_db, this->_vocab, probFn, depProbFn);
    }

    const T nullValue() const; // dependant on spec

    /**
     * @brief vocabulary Return the vocabulary (i.e. unique tokens) in the database model.
     * @return Valid hash set, but may be empty
     */
    const std::unordered_set<T>& vocabulary() const noexcept {
        return _vocab;
    }

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
            return _unfoundProb;
        }
    }
    
    const double depProb(const NGram<T> &ngram) const {
        const auto it = _db.find(ngram);
        if (it != _db.end()) {
            return it->second.depProb;
        } else {
            return _unfoundDepProb;
        }
    }

    const double sentenceDepProb(const std::vector<T> &sentence) const;
    
    const std::vector<double> getNProbs(const std::vector<const NGram<T>* >& pngrams) const noexcept {
        std::vector<NGram<T> > ngrams;
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

    const std::pair<std::vector<size_t>, std::vector<size_t>> rateToCountMapping() const {
        std::unordered_map<size_t, size_t> buff;

        // use a map instead of a vector of pairs because we want lookup
        for (const auto &p : _rateToNGrams) {
            buff[p.first] = p.second.size();
        }

        // now convert to the vector of pairs
        std::vector<std::pair<size_t, size_t> > vbuff(buff.begin(), buff.end());

        // sort on the 'first' or 'key' which is the rate
        std::sort(vbuff.begin(), vbuff.end(), [](const std::pair<size_t, size_t>& a,
                  const std::pair<size_t, size_t>& b) {
            return a.first < b.first;
        });

        std::vector<size_t> rs, ns;
        rs.reserve(vbuff.size());    ns.reserve(vbuff.size());

        for (const auto& p: vbuff) {
            rs.push_back(p.first);
            ns.push_back(p.second);
        }

        return {rs, ns};
    }
    
    /**
     * Get the amount of NGrams with `rate` count. 
     * @param rate Rate to gather.
     * @return Number of NGrams with rate, `rate`.
     */
    const size_t countNGramsWithRate(const size_t rate) const {
        const auto range = _rateToNGrams.equal_range(rate);
        return std::distance(range.first, range.second);
    }
    
    /**
     * Return the NGram<T>'s with count. 
     * @param count Amount to search for. 
     * @return vector of NGrams with count `count`. 
     */
    const std::vector<const NGram<T>*> ngramsWithRate(const size_t rate) const {
        std::vector<const NGram<T>*> out;
        
        const auto it = _rateToNGrams.find(rate);
        if (it == _rateToNGrams.end()) {
            return out;
        }

        out.reserve(it->second.size());
        out.insert(out.begin(), it->second.begin(), it->second.end());

        return out;
    }

    /**
     * Get all the tokens for a database, this is not a unique set.
     * @return vector of tokens. 
     */
    const std::vector<T>& tokens() const noexcept {
        return _allTokens;
    }
    
    /**
     * Gets all of the sentences of the current database. If the model is a `char` model, the @param len parameter is 
     * required to extract the size of the sentence vectors. 
     * @return Sentences of the database. 
     */
    const std::vector<std::vector<T> > sentences(const size_t len = -1) const;

    /**
     * Return the N value used to construct this database.
     * @return N
     */
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
        std::unordered_set<T> vocab;

        DataValue dv;
        for (const auto& pair: other._db) {
            if (_db.count(pair.first) >= 1) {
                dv = pair.second;
                dv.count += _db.find(pair.first)->second.count;
                out[pair.first] = dv;
                
                vocab.insert(pair.first.context().begin(), pair.first.context().end());
                vocab.insert(pair.first.value());
            }
        }

        return Database<T>(_n, std::move(out), std::move(vocab), _probFn, _depProbFn);
    }

    /**
     * @brief notIn returns a Database instance with all of the ngrams in `this` that are not in `other`.
     * @param other
     * @return
     */
    const Database<T> notIn(const Database<T> &other) const {
        std::unordered_map<NGram<T>, DataValue> out;
        std::unordered_set<T> vocab;

        for (const auto& pair: _db) {
            if (other._db.count(pair.first) == 0) {
                out[pair.first] = pair.second;
                vocab.insert(pair.first.context().begin(), pair.first.context().end());
                vocab.insert(pair.first.value());
            }
        }

        return Database<T>(_n, std::move(out), std::move(vocab), _probFn, _depProbFn);
    }

private:

    Database(const std::size_t N, const std::vector<T>& tokens,
             Database<T>::ProbFunc probFn,
             Database<T>::DependantProbFunc depProbFn)
        : _n(N),
          _probFn(probFn),
          _depProbFn(depProbFn),
          _allTokens(tokens) {
        intialize();
        initPostDb();
    }

    Database(const std::size_t N, const std::unordered_map<NGram<T>, DataValue>&& copy,
             const std::unordered_set<T>&& vocab,
             Database<T>::ProbFunc probFn,
             Database<T>::DependantProbFunc depProbFn)
        : _n(N),
          _probFn(probFn),
          _depProbFn(depProbFn),
          _db(copy),
          _vocab(vocab) {


        initPostDb();
    }

    Database(const std::size_t N, const std::unordered_map<NGram<T>, DataValue>& copy,
             const std::unordered_set<T>& vocab,
             Database<T>::ProbFunc probFn,
             Database<T>::DependantProbFunc depProbFn)
        : _n(N),
          _probFn(probFn),
          _depProbFn(depProbFn),
          _db(copy),
          _vocab(vocab) {
        initPostDb();
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
    
    void initPostDb() {
        size_t count = 0;
        auto maxCount = std::numeric_limits<std::size_t>::min();
        
        std::vector<const NGram<T> *> ngrams;
        ngrams.reserve(_db.size());
        std::vector<size_t> ngramCountPerLevel(_n + 1, 0);
        
        std::unordered_map<size_t, std::vector<const NGram<T>*> > count2NGram;
        count2NGram.reserve(_db.size());
                
        for (const auto& p: _db) {
            count += p.second.count;
            
            maxCount = std::max(maxCount, p.second.count);
            ngrams.push_back(&p.first);
            
            ngramCountPerLevel[p.first.n()] += p.second.count;

            auto c2nit = count2NGram.find(p.second.count);
            if (c2nit == count2NGram.end()) {
                count2NGram[p.second.count] = std::vector<const NGram<T>*>();
            }

            count2NGram[p.second.count].push_back(&p.first);
        }

        ngrams.shrink_to_fit();
        _ngrams = std::move(ngrams);
        _ngramsPerLevel = std::move(ngramCountPerLevel);
        _maxCount = maxCount;
        _rateToNGrams = std::move(count2NGram);
        
        // calculate the probability of occurance
        for (auto& p : _db) {
            p.second.prob = _probFn(*this, p.first, p.second.count);
        }
        _unfoundProb = _probFn(*this, NGram<T>(this->nullValue()), 0);
        
        // compute the dependant probabilities
        for (auto& p : _db) {
            p.second.depProb = _depProbFn(*this, p.first, p.second.count);
        }
        _unfoundDepProb = _depProbFn(*this, NGram<T>(this->nullValue()), 0);
    }
    
    template <typename Iter>
    const double __sentenceDepProb(Iter senStart, const Iter senEnd) const {
        const int dist = std::distance(senStart, senEnd);
        if (dist <= 0)  {
            return 0.0;
        }

        double out = 0.0;
        
        auto it = senStart;
        NGram<T> prev{ *it };
        out += std::log(this->depProb(prev));
        
        for (size_t i = 1; i < std::min(this->n(), size_t(dist - 1)); ++i) {
            prev = prev.asContext(*(++it), i+1);
            out += std::log(this->depProb(prev));
        }
        
        do {
            prev = prev.asContext(*(++it));
            out += std::log(this->depProb(prev));
        } while (it != (senEnd - 1));

        return out;
    }

    const size_t _n;
    const ProbFunc _probFn;
    const DependantProbFunc _depProbFn;

    std::unordered_map<NGram<T>, DataValue> _db;
    std::vector<const NGram<T> *> _ngrams;
    std::vector<size_t> _ngramsPerLevel;
    std::vector<T> _allTokens;
    std::unordered_set<T> _vocab;
    std::unordered_map<size_t, std::vector<const NGram<T>*> > _rateToNGrams;
    size_t _maxCount;

    double _unfoundProb;
    double _unfoundDepProb;

    friend class DatabaseFactory;
};

template <>
const std::string Database<std::string>::nullValue() const {
    return "";
}

template <>
const char Database<char>::nullValue() const {
    return '\0';
}

template <>
const std::vector<std::vector<std::string> > Database<std::string>::sentences(const size_t) const {
    return parseSentences(_allTokens);
}

template <>
const std::vector<std::vector<char> > Database<char>::sentences(const size_t len) const {
    return parseSentences(_allTokens, len);
}

template <>
const double Database<std::string>::sentenceDepProb(const std::vector<std::string> &sentence) const {
    // be sure to ignore the <END> if its there
    const auto endIt = sentence.end() - (*(sentence.end() - 1) == EOS_TERM ? 1 : 0);
    return __sentenceDepProb(sentence.begin(), endIt);
}

template <>
const double Database<char>::sentenceDepProb(const std::vector<char> &sentence) const {
    return __sentenceDepProb(sentence.begin(), sentence.end());
}


namespace NGramProbFunc {

    template <typename T>
    static double simpleProbability(const Database<T>& db, const NGram<T>& that, // unused params
                                    size_t count) {
        return (1.0 * count) / db.ngramCount(that.n());
    }

    template <typename T>
    static double mle(const Database<T>& db, const NGram<T>& ngram, const size_t count) {
        if (count == 0 || ngram.n() == 1) { // no such thing as a 0-gram..
            const auto prob = db.prob(ngram);
            return prob;
        } else {
            const auto ctxcount = db.count(ngram.contextNgram());
            if (ctxcount > 0) {
                return (1.0 * count) / ctxcount;
            } else {
                return db.prob(ngram);
            }
        }
    }

    template <typename T>
    static double dependantProb(const Database<T>&db, const NGram<T>& that, const size_t) {
         const auto prob = db.prob(that);

         if (that.n() > 1) {
             const auto oprob = db.prob(that.contextNgram());
             return prob / oprob;
         }

         return prob;
     }
}

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
    static Database<U> create(const size_t N, const std::unordered_map<NGram<U>, typename Database<U>::DataValue>& copy,
                              typename Database<U>::ProbFunc probFn = &NGramProbFunc::simpleProbability<U>,
                              typename Database<U>::DependantProbFunc depProbFn = &NGramProbFunc::mle<U>) {
        return Database<U>(N, copy, probFn, depProbFn);
    }
};

} // end kbright2 namespace


#endif // __DATABASE_H

