#include <bits/stdc++.h>

using namespace std;

template<typename T, const T& (*Func)(const T&, const T&)>
struct sparse_table {
    vector<vector<T>> sparse;
    sparse_table() {}
    sparse_table(const vector<T>& initial_array) {
        build(initial_array);
    }
    void build(const vector<T>& initial_array) {
        size_t j_size = __lg(initial_array.size()) + 1;
        sparse.resize(j_size, vector<T>(initial_array.size()));
        for (size_t i = 0; i < initial_array.size(); ++i) {
            sparse[0][i] = initial_array[i];
        }
        size_t offset = 1;
        for (size_t j = 1; j < j_size; ++j) {
            for (size_t i = 0; i + offset < initial_array.size(); ++i) {
                sparse[j][i] = Func(sparse[j - 1][i], sparse[j - 1][i + offset]);
            }
            offset <<= size_t(1);
        }
    }
    T query(size_t l, size_t r) {  // [l; r)
        size_t x = __lg(r - l);
        return Func(sparse[x][l], sparse[x][r - (1 << x)]);
    }
};