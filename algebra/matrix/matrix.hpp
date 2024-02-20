#ifndef MATRIX_HPP
#define MATRIX_HPP
#include <stdint.h>

#include <utility>

template <typename T, uint64_t w, uint64_t h = w>
class matrix {
   private:
    T _elements[h][w];

   public:
    matrix() {}

    matrix(T initialValue) {
        for (uint64_t j = 0; j < h; j++) {
            for (uint64_t i = 0; i < w; i++) {
                this->_elements[j][i] = initialValue;
            }
        }
    }

    matrix(T elements[h][w]) {
        for (uint64_t j = 0; j < h; j++) {
            for (uint64_t i = 0; i < w; i++) {
                this->_elements[j][i] = elements[j][i];
            }
        }
    }

    ~matrix() {}

    void fill(T initialValue) {
        for (uint64_t j = 0; j < h; j++) {
            for (uint64_t i = 0; i < w; i++) {
                this->_elements[j][i] = initialValue;
            }
        }
    }

    // ********************************************
    // * OPERATORS                                *
    // ********************************************

    void operator+=(const matrix<T, w, h> other) {
        for (uint64_t y = 0; y < h; y++) {
            for (uint64_t x = 0; x < w; x++) {
                this->_elements[y][x] += other._elements[y][x];
            }
        }
    }

    void operator-=(const matrix<T, w, h> other) {
        for (uint64_t y = 0; y < h; y++) {
            for (uint64_t x = 0; x < w; x++) {
                this->_elements[y][x] -= other._elements[y][x];
            }
        }
    }

    void operator=(T elements[h][w]) {
        for (uint64_t j = 0; j < h; j++) {
            for (uint64_t i = 0; i < w; i++) {
                this->_elements[j][i] = elements[j][i];
            }
        }
    }

    // ********************************************
    // * ACCESS FUNCTIONS                         *
    // ********************************************

    T* operator[](uint64_t i) { return this->_elements[i]; }
    const T* operator[](uint64_t i) const { return this->_elements[i]; }
};

// ********************************************
// * OPERATORS                                *
// ********************************************

template <typename T, uint64_t w1, uint64_t h1, uint64_t w2>
matrix<T, w2, h1> operator*(const matrix<T, w1, h1>& left,
                            const matrix<T, w2, w1>& right) {
    matrix<T, w2, h1> out(T(0));

    for (uint64_t y = 0; y < h1; y++) {
        for (uint64_t x = 0; x < w1; x++) {
            const T el = left[y][x];

            for (uint64_t i = 0; i < w2; i++) {
                out[y][i] += el * right[x][i];
            }
        }
    }

    return out;
}

template <typename T, uint64_t w, uint64_t h>
bool operator==(const matrix<T, w, h>& left, const matrix<T, w, h>& right) {
    for (uint64_t y = 0; y < h; y++) {
        for (uint64_t x = 0; x < w; x++) {
            if (left[y][x] != right[y][x]) return false;
        }
    }

    return true;
}

// ********************************************
// * ALGEBRA                                  *
// ********************************************

template <typename T, uint64_t s>
matrix<T, s> identity() {
    matrix<T, s> out(T(0));

    for (uint64_t i = 0; i < s; i++) {
        out[i][i] = T(1);
    }

    return out;
}

template <typename T, uint64_t w1, uint64_t h1, uint64_t w2>
matrix<T, w2, h1>& multiplyTo(const matrix<T, w1, h1>& left,
                              const matrix<T, w2, w1>& right,
                              matrix<T, w2, h1>& out) {
    out.fill(T(0));

    for (uint64_t y = 0; y < h1; y++) {
        for (uint64_t x = 0; x < w1; x++) {
            const T el = left[y][x];

            for (uint64_t i = 0; i < w2; i++) {
                out[y][i] += el * right[x][i];
            }
        }
    }

    return out;
}

template <typename T, typename U, uint64_t s>
matrix<T, s> binpow(const matrix<T, s>& x, U power) {
    if (power == 1) {
        return x;
    }

    matrix<T, s> powered = x;
    matrix<T, s> out = identity<T, s>();

    matrix<T, s> outBuff(T(0));
    matrix<T, s> poweredBuff(T(0));

    while (power > 0) {
        if (power & 1) {
            multiplyTo(out, powered, outBuff);
            std::swap(out, outBuff);
        }

        multiplyTo(powered, powered, poweredBuff);
        std::swap(powered, poweredBuff);
        power >>= 1;
    }

    return out;
};

template <typename T, typename U, uint64_t s>
void binpowInPlace(matrix<T, s>& x, U power) {
    if (power == 1) {
        return;
    }

    power--;

    matrix<T, s> powered = x;

    matrix<T, s> outBuff(T(0));
    matrix<T, s> poweredBuff(T(0));

    while (power > 0) {
        if (power & 1) {
            multiplyTo(x, powered, outBuff);
            std::swap(x, outBuff);
        }

        multiplyTo(powered, powered, poweredBuff);
        std::swap(powered, poweredBuff);
        power >>= 1;
    }
};

#endif