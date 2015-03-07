#ifndef __DATABASE_H
#define __DATABASE_H

#include <string>
#include <unordered_map>
#include <vector>
#include "VectorHash.h"
#include "fileRead.h"

namespace kbright2 {

template <typename T>
class Database {

public:

    static Database<T> createFromFile(const size_t N, const std::string &path, const bool readEOS = false) {
        std::vector<std::string> tokens;
        read_tokens(path, tokens, readEOS);

        return new Database<T>(N, tokens);
    }

    static Database<T> create(const size_t N, const std::unordered_map<std::vector<T>, std::size_t> &copy) {
        return new Database<T>(N, copy);
    }

    static Database<T> createMultiGram(const size_t N, const std::string &path, const bool readEOS = false) {
        std::vector<std::string> tokens;
        read_tokens(path, tokens, readEOS);

        return new Database<T>(N, tokens);
    }

    const std::size_t maxCount() const noexcept {
        return _maxCount;
    }

    const std::unordered_map<std::vector<T>, std::size_t>& db() const {
        return _db;
    }

    const std::vector<std::vector<T>> &ngrams() const {
        return _ngrams;
    }

    std::vector<T>& tokens() const {
        return _allTokens;
    }

    const std::size_t n() const noexcept {
        return _n;
    }

    /**
     * @brief intersect Performs an intersection of `this` with `other`.
     * @param other
     * @return
     */
    const Database<T> intersect(const Database<T> &other) const {
        std::unordered_map<std::vector<T>, std::size_t> out;

        for (const auto &pair: other.db()) {
            if (_db.count(pair.first) >= 1) {
                out[pair.first] = pair.second + _db[pair.first];
            }
        }

        return Database<T>(_n, out);
    }

    /**
     * @brief notIn returns a Database instance with all of the ngrams in `this` that are not in `other`.
     * @param other
     * @return
     */
    const Database<T> notIn(const Database<T> &other) const {
        std::unordered_map<std::vector<T>, std::size_t> out;

        for (const auto &pair: _db) {
            if (other.db().count(pair.first) != 1) {
                out[pair.first] = pair.second;
            }
        }

        return Database<T>(_n, out);
    }

private:

    Database(const std::size_t N, const std::vector<std::string> &tokens)
        : _n(N) {
        intialize(tokens);
        initNgramVector();
    }

    Database(const std::size_t N, const std::unordered_map<std::vector<T>, std::size_t> &copy)
        : _n(N),
          _db(copy) {
        initNgramVector();
    }

    ~Database() { }

    void intialize(const std::vector<std::string> &tokens, const bool ) {

        if (_allTokens.size() < _n) {
            throw std::out_of_range("Tokens read < minimum ngram size.");
        }

        _maxCount = std::numeric_limits<std::size_t>::min();

        for ( size_t i = 0; i <= _allTokens.size() - _n; ++i )
        {
            std::vector<T> nGram{ _n };   // for temporarily storing tokens to go into next n-gram

            // Take next n tokens read from the input file
            for ( size_t j = 0; j < _n; j++ ) {
                nGram[j] = _allTokens[i+j]; // put next n tokens into vector temp
            }

            if ( _db.count(nGram) == 0 ) { // nGram is not in the database yet, insert it with count 1
                _db[nGram] = 1;
            } else { // nGram is already in the database, increase its count by 1
                _db[nGram] += 1;
            }

            _maxCount = std::max(_maxCount, _db[nGram]);
        }
    }

    void initNgramVector() {
        if (_db.size() == 0) {
            return;
        }

        _ngrams = std::vector<std::vector<T>>(_db.size());

        size_t i = 0;
        for (const auto &p : _db) {
            _ngrams[i++] = p.first;
        }
    }

    const std::size_t _n;
    const std::string _path;

    std::unordered_map<std::vector<T>, std::size_t> _db;
    std::vector<std::vector<T>> _ngrams;
    std::vector<T> _allTokens;
    std::size_t _maxCount;
};


}


#endif // __DATABASE_H

