#ifndef SEGTREE_HPP
#define SEGTREE_HPP
#include <stdint.h>

template <typename T,
          void (*_operation_func)(T& result, const T& left, const T& right)>
class SegmentTree {
   private:
    T* segments;
    uint32_t size;

   public:
    /**
     * @param size - size of the array to build segment tree from
     * @param func - function that performs some operation with two values. E.g.
     *               for summing segment tree you need to provide a summator.
     */
    SegmentTree(uint32_t size) {
        this->size = size;
        this->segments = new T[size * 2 - 1];
    }

    ~SegmentTree() { delete[] this->segments; }

    /**
     * Fills Segment Tree with provided values.
     * You can initialize value in segtree with that.
     */
    void fillup(const T* array) {
        for (uint32_t i = 0; i < size; i++) {
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
        for (int32_t i = size - 2; i >= 0; i--) {
            _operation_func(this->segments[i], this->segments[i * 2 + 1],
                            this->segments[i * 2 + 2]);
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

        for (; currentIndex >= 0; currentIndex = (currentIndex + 1) / 2 - 1) {
            _operation_func(this->segments[currentIndex],
                            this->segments[currentIndex * 2 + 1],
                            this->segments[currentIndex * 2 + 2]);
        }
    }

    /**
     * Gets result for operation at range [l; r) ( including l and excluding r )
     * Has logarithmic complexity
     * @param l - left boundary ( inclusive )
     * @param r - right boundary ( exclusive )
     * @param initialValue - value passed to first call of queryUpdate_func.
     */
    template <typename _QueryResult, typename... _Args>
    _QueryResult operate(int32_t l, int32_t r, _QueryResult initialValue,
                         void (*queryUpdate_func)(_QueryResult& out,
                                                  const T& segment,
                                                  _Args... args),
                         _Args... args) const {
        // Segments are organised specific way, where last segments
        // represent individual elements ( i.e. they have length 1 ).
        // In the algorithm we need to start from them
        l += this->size - 1;
        r += this->size - 1;

        _QueryResult result = initialValue;

        while (l < r) {
            if (l % 2 == 0) {
                queryUpdate_func(result, (const T)this->segments[l], args...);
            }

            if (r % 2 == 0) {
                queryUpdate_func(result, (const T)this->segments[r - 1],
                                 args...);
            }

            l /= 2;
            r = (r - 1) / 2;
        }

        return result;
    };
};

#endif