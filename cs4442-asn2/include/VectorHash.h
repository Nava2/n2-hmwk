#ifndef __VectorHash_H
#define __VectorHash_H

#include <vector>

////////////////////////////////////////////////////////////////
// for unordered_map with vector<string> key, have to implement
// custom function that computes hash code
// class for vector<string> hash code computation

namespace std {

	template<class T>
	struct hash < std::vector<T> > {
		size_t operator()(const std::vector<T>& vec) const {
			size_t res = 0;
            for (const auto &v : vec)
                res = res * 37 + internal_(v);
			return res;
		}

		std::hash<T> internal_;
	};

}

#endif

