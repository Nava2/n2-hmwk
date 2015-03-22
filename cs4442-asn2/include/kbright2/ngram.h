#ifndef __NGRAM_H
#define __NGRAM_H

#include <vector>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <math.h>
#include <stdexcept>

namespace kbright2 { 
    
template<typename T>
class NGram {
    
public:
    
    NGram(std::initializer_list<T> l)
        : NGram(*(l.end() - 1), std::vector<T>(l.begin(), l.end() - 1)) {
        assert(l.size() > 0);
    }
    
    NGram(const T& val, const std::vector<T>& context = std::vector<T>(0))
        : _N(context.size() + 1), _val(val), _context(context) {
        // no need to assert since val can't be null
    }
    
    template<typename Iter>
    NGram(const Iter& val, const Iter& contextBegin, const Iter& contextEnd) 
        : NGram(*val, std::vector<T>(contextBegin, contextEnd)) {
    }

    NGram(const std::vector<T>& buff) 
        : NGram(buff.end() - 1, buff.begin(), buff.end() - 1) {
    }
    
    NGram(const NGram<T> &other)
        : _N(other._N), _val(other._val), _context(other._context) {
    }
    
    NGram(const NGram<T>&& other)
        : _N(other._N), _val(std::move(other._val)), _context(std::move(other._context)) {
        
    }
    
    NGram<T>& operator=(const NGram<T>&& other) {
        this->_context = std::move(other._context);
        this->_val = std::move(other._val);
        this->_N = std::move(other._N);
        
        return *this;
    }
    
    ~NGram() { }
    
    bool operator==(const NGram &other) const noexcept{
        return (this->_N == other._N && 
            this->_val == other._val && 
            this->_context == other._context);
    }
    
    const size_t n() const noexcept {
        return _N;
    }
    
    const std::vector<T>& context() const noexcept {
        return _context;
    }
    
    const T &value() const noexcept {
        return _val;
    }
  
    const NGram<T> contextNgram() const {
        if (_N == 1) {
            throw std::invalid_argument("N == 1, can not create context NGram");
        }
        return NGram<T>(_context);
    }

    const NGram<T> asContext(const T& value, const int sizeN = -1) const {
        if (sizeN == 0 || sizeN > int(n() + 1)) {
           throw std::invalid_argument("sizeN must be default or between [1..this->n() + 1].");
        }
        
        const int newN = sizeN == -1 ? n() : sizeN;
        
        std::vector<T> context;
        context.reserve(std::max(0, newN - 1));
        if (newN > 1) {
            context.insert(context.begin(), _context.begin() + std::max(0, (int(this->n()) - int(newN) + 1)), _context.end());
            context.insert(context.end(), this->value());
        }

        return NGram<T>(value, context);
    }
    
    const std::string toString() const {
        std::stringstream ss;
        for (const auto& v: _context) {
            ss << v << ' ';
        }
        ss << _val;
        
        return ss.str();
    }
    
    static 
    std::vector<NGram<T> > createCombinations(const std::vector<T>& values, const std::vector<T>& context) {
        std::vector<NGram<T> > out;
        out.reserve(values.size());
        for (const auto &v : values) {
            out.push_back(NGram<T>(v, context));
        }
        
        return out;
    }
    
    static 
    std::vector<NGram<T> > createCombinations(const std::vector<T>& values, const NGram<T>& context, int sizeN = -1) {
        std::vector<NGram<T> > out;
        out.reserve(values.size());
        for (const auto &v : values) {
            out.push_back(context.asContext(v, sizeN));
        }
        
        return out;
    }
    
private:
    
    size_t _N;
    T _val;
    std::vector<T> _context;
    
};

template <typename U>
std::ostream& operator<<(std::ostream& os, const NGram<U>& obj)
{
    // write obj to stream
    os << "NGram{" << obj.value();
    
    const auto &context = obj.context();
    if (context.size() > 0) {
        os << "|";
    
        size_t i = 0;
        for (; i < context.size() - 1; ++i) {
            os << context[i] << ",";
        }
        
        os << context[i];
    }
    
    os << "}";
    
    return os;
}

} // end kbright2

namespace std {
    
template<typename T>
struct hash<kbright2::NGram<T>>
{
  
    size_t operator()(kbright2::NGram<T> const& s) const
    {
        size_t result = 17;
        result = result * 31 + ( std::hash<T>()(s.value()) );
        result = result * 31 + ( std::hash<vector<T> >()(s.context()) );
        return result;
    }
};

}


#endif
