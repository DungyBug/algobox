#ifndef MATRIX_HPP
#define MATRIX_HPP
#include <stdint.h>

#include <utility>

template <typename T, std::size_t w, std::size_t h = w>
class matrix {
   private:
    T _elements[h][w];

   public:
    matrix() {}

    matrix(T initialValue) {
        for (std::size_t j = 0; j < h; j++) {
            for (std::size_t i = 0; i < w; i++) {
                this->_elements[j][i] = initialValue;
            }
        }
    }

    matrix(T elements[h][w]) { this->_elements = elements; }

    ~matrix() {}

    inline void fill(T initialValue) {
        for (std::size_t j = 0; j < h; j++) {
            for (std::size_t i = 0; i < w; i++) {
                this->_elements[j][i] = initialValue;
            }
        }
    }

    // ********************************************
    // *                OPERATORS                 *
    // ********************************************

    void operator+=(const matrix<T, w, h> other) {
        for (std::size_t y = 0; y < h; y++) {
            for (std::size_t x = 0; x < w; x++) {
                this->_elements[y][x] += other._elements[y][x];
            }
        }
    }

    void operator-=(const matrix<T, w, h> other) {
        for (std::size_t y = 0; y < h; y++) {
            for (std::size_t x = 0; x < w; x++) {
                this->_elements[y][x] -= other._elements[y][x];
            }
        }
    }

    void operator=(T elements[h][w]) {
        for (std::size_t j = 0; j < h; j++) {
            for (std::size_t i = 0; i < w; i++) {
                this->_elements[j][i] = elements[j][i];
            }
        }
    }

    // ********************************************
    // *             ACCESS FUNCTIONS             *
    // ********************************************

    inline T* operator[](std::size_t i) { return this->_elements[i]; }
    inline const T* operator[](std::size_t i) const {
        return this->_elements[i];
    }
};

// ********************************************
// *                OPERATORS                 *
// ********************************************

/**
 * @attention For frequent multiplication consider using `multiplyTo` with
 * buffer matrices as it does not create new matrix and therefore is more
 * memory-efficient ( see matrix `binpow` implementation for usage example )
 */
template <typename T, std::size_t w1, std::size_t h1, std::size_t w2>
matrix<T, w2, h1> operator*(const matrix<T, w1, h1>& left,
                            const matrix<T, w2, w1>& right) {
    matrix<T, w2, h1> out(T(0));

    for (std::size_t y = 0; y < h1; y++) {
        for (std::size_t x = 0; x < w1; x++) {
            const T el = left[y][x];

            for (std::size_t i = 0; i < w2; i++) {
                out[y][i] += el * right[x][i];
            }
        }
    }

    return out;
}

template <typename T, std::size_t w, std::size_t h>
bool operator==(const matrix<T, w, h>& left, const matrix<T, w, h>& right) {
    for (std::size_t y = 0; y < h; y++) {
        for (std::size_t x = 0; x < w; x++) {
            if (left[y][x] != right[y][x]) return false;
        }
    }

    return true;
}

// ********************************************
// *                 ALGEBRA                  *
// ********************************************

/**
 * @brief Creates identity *square* matrix
 */
template <typename T, std::size_t s>
matrix<T, s> identity() {
    matrix<T, s> out(T(0));

    for (std::size_t i = 0; i < s; i++) {
        out[i][i] = T(1);
    }

    return out;
}

/**
 * @brief Multiplies two matrices and saves the result into the third one.
 * @attention For frequent multiplication using this function is recommended as
 * it doesn't allocate memory and therefore is memory-efficient
 * @param out - where to write the result
 * @returns reference to out for convenient use ( BUT NOT a new matrix )
 */
template <typename T, std::size_t w1, std::size_t h1, std::size_t w2>
matrix<T, w2, h1>& multiplyTo(const matrix<T, w1, h1>& left,
                              const matrix<T, w2, w1>& right,
                              matrix<T, w2, h1>& out) {
    out.fill(T(0));

    for (std::size_t y = 0; y < h1; ++y) {
        for (std::size_t x = 0; x < w1; ++x) {
            const T el = left[y][x];

            for (std::size_t i = 0; i < w2; ++i) {
                out[y][i] += el * right[x][i];
            }
        }
    }

    return out;
}

/**
 * @brief Binary raises matrix to a power. Only square matrix can be raised to
 * power, as only square matrix can be multiplied by self.
 * @param x - matrix to raise to power to
 * @param power - power, should be some of integer type
 * @returns raised to power matrix
 */
template <typename T, typename U, std::size_t s>
matrix<T, s> binpow(matrix<T, s> x, U power) {
    if (power == 1) {
        return x;
    }

    matrix<T, s> out = identity<T, s>();
    matrix<T, s> buff;

    // Usual binpow that goes through binary represenation of power

    while (power > 0) {
        // For some reason, adding an "else" branch makes this code
        // faster, despite else branch is guaranteed to execute in the
        // next cycle ( so, logically, we should loose cycles rather than gain
        // them )
        if (power % 2) {
            multiplyTo(out, x, buff);
            std::swap(out, buff);
            power--;
        } else {
            multiplyTo(x, x, buff);
            std::swap(x, buff);
            power /= 2;
        }
    }

    return out;
};

/**
 * @brief Mutates and binary raises matrix to a power. Only square matrix can be
 * raised to power, as only square matrix can be multiplied by self.
 * @param x - matrix to raise to power to ( note that the matrix is mutated! )
 * @param power - power, should be some of integer type
 * @note works usually slower than `binpow`
 */
template <typename T, typename U, std::size_t s>
void binpowInPlace(matrix<T, s>& x, U power) {
    if (power == 1) {
        return;
    }

    power--;

    matrix<T, s> powered = x;
    matrix<T, s> buff(T(0));

    while (power > 0) {
        if (power & 1) {
            multiplyTo(x, powered, buff);
            std::swap(x, buff);
        }

        multiplyTo(powered, powered, buff);
        std::swap(powered, buff);
        power >>= 1;
    }
};

#endif