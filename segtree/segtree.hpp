#ifndef SEGTREE_HPP
#define SEGTREE_HPP
#include <stdint.h>

template <typename T>
struct segtree_t {
    typedef void (*_operation_func)(T& result, const T& left, const T& right);

    T* segments;
    uint32_t size;
    _operation_func operation_func;

    /**
     * @param size - size of the array to build segment tree from
     * @param func - function that performs some operation with two values. E.g.
     *               for summing segment tree you need to provide a summator.
    */
    segtree_t(uint32_t size, _operation_func func) {
        this->size = size;
        this->segments = new T[size * 2 - 1];
        this->operation_func = func;
    }

    ~segtree_t() {
        delete[] this->segments;
    }

    /**
     * Fills Segment Tree with provided values.
     * You can initialize value in segtree with that.
    */
    void fillup(const T* array) {
        for(uint32_t i = 0; i < size; i++) {
            this->segments[i + size - 1] = array[i];
        }

        this->updateSegments();
    }

    /**
     * Changes value without updating segments. Helpful for filling segtree
     * without arrays.
     * You need to call `updateSegments` before calling `operate`
    */
    void setValueWithoutUpdate(uint32_t index, T value) {
        this->segments[index + this->size - 1] = value;
    }

    /**
     * Updates segments, so you can use `operate`.
     * Call this before `operate` if you'd used `setValueWithoutUpdate`
    */
    void updateSegments() {
        for(int32_t i = size - 2; i >= 0; i--) {
            this->operation_func(this->segments[i], this->segments[i * 2 + 1], this->segments[i * 2 + 2]);
        }
    }

    /**
     * It does what you think it does.
     * Has logarithmic complexity
    */
    void setValue(uint32_t index, T value) {
        int32_t currentIndex = index + this->size - 1;

        this->segments[currentIndex] = value;

        currentIndex = (currentIndex + 1) / 2 - 1;

        for(; currentIndex >= 0; currentIndex = (currentIndex + 1) / 2 - 1) {
            this->operation_func(this->segments[currentIndex], this->segments[currentIndex * 2 + 1], this->segments[currentIndex * 2 + 2]);
        }
    }

    /**
     * Gets result for operation at range [l; r) ( including l and excluding r )
     * Has logarithmic complexity
     * @param l - left boundary ( inclusive )
     * @param r - right boundary ( exclusive )
     * @param initialValue - should be used, when segtree operates complex types,
     *                       such as structs and classes, which cannot be left
     *                       unitialized. On first call of `operation_func` this
     *                       `initialValue` is passed as second argument.
    */
    T operate(int32_t l, int32_t r, T initialValue = 0) {
        // Segments are organised specific way, where last segments
        // represent individual elements ( i.e. they have length 1 ).
        // In the algorithm we need to start from them
        l += this->size - 1;
        r += this->size - 1;

        T result = initialValue;

        while(l < r) {
            if(l % 2 == 0) {
                this->operation_func(result, result, this->segments[l]);
            }

            if(r % 2 == 0) {
                this->operation_func(result, result, this->segments[r - 1]);
            }

            l /= 2;
            r = (r - 1) / 2;
        }

        return result;
    }
};

#endif