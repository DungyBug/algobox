#ifndef RAGE_HPP
#define RAGE_HPP

#include "../radix/radix.hpp"
#include <functional>
#include <iostream>
#include <stdint.h>

namespace RageSort {

template <typename T, typename _Compare = std::less<T>>
inline void merge(const T *begin1, const T *begin2, const T *end2, T *out,
                  _Compare comp = std::less<T>()) {
    const uint64_t size1 = begin2 - begin1;
    const uint64_t size2 = end2 - begin2;

    uint64_t l = 0;
    uint64_t r = 0;
    uint64_t i = 0;

    while (l < size1 && r < size2) {
        const uint64_t left = begin1[l];
        const uint64_t right = begin2[r];

        // TODO: Use previously generated keys to compare elements to get rid of
        // operator<
        if (comp(left, right)) {
            out[i] = left;
            l++;
        } else {
            out[i] = right;
            r++;
        }

        i++;
    }

    while (l < size1) {
        out[i] = begin1[l];

        l++;
        i++;
    }

    while (r < size2) {
        out[i] = begin2[r];

        r++;
        i++;
    }
}

} // namespace RageSort

/**
 * @brief Sorts complex objects which are convertable to some numeric key. Has
 * complexity of O(nwlognw) in worst case, where n is size of the array and w -
 * count of bytes in numeric key. Average complexity is O(nw).
 * It's a radix sort with better cache utilization by using merge sort in last
 * steps.
 * @param begin - pointer to the array's first element
 * @param end - pointer to the element above last
 * @param out - location of allocated data to place sorted array to. Should NOT
 * point to the same location as `begin` points to.
 * @param keyFunc - `void keyFunc(T& element, void** outkey, uint32_t* keylen)`
 * - type to numeric key converter. Accepts element and writes to outkey a
 * sequence of bytes so it becomes a n-bit key and to keylen length of that
 * sequence in bytes. Note that provided outkey points to a NON-ALLOCATED data
 * so you need to allocate it first.
 * @param freeKeys - if true, then all keys returned by keyFunc are freed by
 * delete operator.
 * @note It performs best when keys and objects are lightweight. Consider using
 * std::sort or radix sort for heavy objects.
 */
template <typename T, typename U, typename _Compare = std::less<T>>
void rageSort(T *begin, T *end, T *out, const U &keyFunc, bool freeKeys = false,
              _Compare comp = std::less<T>()) {
    using algobox_p::element_t;

    // May vary depending on cpu cache size. Usually, value somewhere around
    // 100000 is quite good for modern processors.
    const uint64_t STEP_SIZE = 1 << 17;

    const uint64_t arraySize = end - begin;

    // Rage sort with decreased STEP_SIZE shows worse results compared to simple
    // radix sort.
    if (arraySize < STEP_SIZE) {
        radixSort(begin, end, out, keyFunc, freeKeys);
        return;
    }

    // Save original pointer, so we can safely free it later.
    T *buffer = new T[arraySize];

    // Temp will swap next in the code, so don't allocate directly ( or you can
    // free `T* out` pointer by accident )
    T *temp = buffer;
    T *sorted = out;

    // ********************************************
    // Sort first groups of items with radix sort
    // ********************************************

    for (uint64_t i = 0; i < arraySize; i += STEP_SIZE) {
        // Implementation of radix sort here allocates new memory of size
        // STEP_SIZE * 2 each call. This is slow.
        // Never the less, allocating memory once and passing it into function
        // as an argument won't save time, at least for current STEP_SIZE.
        radixSort(begin + i, std::min(end, begin + i + STEP_SIZE), sorted + i,
                  keyFunc, freeKeys);
    }

    // ********************************************
    // Merge sort sorted groups
    // ********************************************

    for (uint64_t k = 1; STEP_SIZE * k < arraySize; k *= 2) {
        std::swap(temp, sorted);

        uint64_t i = 0;

        // Iteratively goes by group pairs and merges them. Cache friendly.
        for (; i + STEP_SIZE * k < arraySize; i += STEP_SIZE * 2 * k) {
            RageSort::merge(
                temp + i, temp + i + STEP_SIZE * k,
                std::min(temp + i + STEP_SIZE * 2 * k, temp + arraySize),
                sorted + i, comp);
        }

        i -= STEP_SIZE * 2 * k;

        // We could left unmerged part that is smaller STEP_SIZE * k ( current
        // step size ). This one wasn't actually supposed to be merged, but it
        // would be nice to sync it between two temporary buffers, so this
        // unmerged part stays sorted in both.
        if (arraySize > i && STEP_SIZE * k * 2 < arraySize) {
            const uint64_t moveSize =
                std::min(arraySize - i, STEP_SIZE * 2 * k);

            // On average, this should take O(n) operations totally
            memcpy(temp + i, sorted + i, sizeof(T) * (moveSize));
        }
    }

    // Copy the result ( and not clone it, as we're just sorting, not generating
    // new sorted list, like std::sort does ). This is the most lightweight part
    // of the code, so doesn't need an optimization here.
    if (out != sorted)
        memcpy(out, sorted, arraySize * sizeof(T));

    // Don't free `temp`, otherwise `free(): invalid pointer`, segmentation
    // fault, bus error or other memory error can occur.
    delete[] buffer;
}

#endif