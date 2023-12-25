#ifndef RADIX_HPP
#define RADIX_HPP
#include <string.h>
#include <stdint.h>
#include <functional>

// Algobox's private namespace
namespace algobox_p {

template <typename T>
struct element_t {
    uint8_t* key;
    uint64_t index;
};

};

template <uint32_t N>
constexpr inline uint64_t getKeyFrom(uint8_t *bytes) {
    uint64_t key = 0;

    for(uint64_t i = N; i > 0; i--) {
        key <<= 8;
        key |= bytes[i - 1];
    }

    return key;
};

/**
 * @brief Sorts complex objects which are convertable to some numeric key. Has complexity of
 *        O(nw), where n is size of the array and w - count of bytes in numeric key.
 * @param begin - pointer to the array's first element
 * @param end - pointer to the element above last
 * @param out - location of allocated data to place sorted array to. Should NOT point to the
 *              same location as `begin` points to.
 * @param keyFunc - `void keyFunc(T& element, void** outkey, uint32_t* keylen)` -
 *                  type to numeric key converter. Accepts element and writes to outkey a
 *                  sequence of bytes so it becomes a n-bit key and to keylen length of that
 *                  sequence in bytes. Note that provided outkey points to a NON-ALLOCATED
 *                  data so you need to allocate it first.
 * @param freeKeys - if true, then all keys returned by keyFunc are freed by delete operator.
 * @return nothing, 
*/
template <uint32_t BYTES = 1, typename T, typename U>
void radixSort(T* begin, T* end, T* out, const U &keyFunc, bool freeKeys = false) {
    using algobox_p::element_t;

    const uint64_t RADIX_SORT_STACK_SIZE = 1ull << (BYTES << 3ull);

    uint64_t arraySize = end - begin;
    uint32_t keySize = 0;

    // *************************************************
    // *                  PREPARATION                  *
    // *************************************************

    element_t<T>* elements = new element_t<T>[arraySize * 2];
    element_t<T>* elementBuffer = elements + arraySize;

    // Save original pointer to elements buffer to be able to
    // free two buffers after changing elements and elementBuffer
    // pointers.
    element_t<T>* elementsBufferOrigin = elements;

    uint32_t currentKeySize;

    for(uint64_t i = 0; i < arraySize; i++) {
        element_t<T> &element = elements[i];

        keyFunc(begin[i], (void**)&element.key, &currentKeySize);
        keySize = std::max(keySize, currentKeySize);

        element.index = i;
    }

    uint64_t counts[RADIX_SORT_STACK_SIZE];

    // *************************************************
    // *                    SORTING                    *
    // *************************************************

    for(uint32_t key = 0; key < keySize; key += BYTES) {
        memset(counts, 0, RADIX_SORT_STACK_SIZE * sizeof(uint64_t));

        // In this implementation of radix sort values are sorted ( by digits )
        // with counting digit occurrences, going through counts array and
        // on i-th step writing back to counts[i] the sum of all `counts` elements
        // from 0 to i.
        // This allows to determine last index of each value, so decrementing
        // that index on each value occurrence we can get sorted values at the
        // end.

        for(uint64_t i = 0; i < arraySize; i++) {
            counts[getKeyFrom<BYTES>(elements[i].key + key)]++;
        }

        for(uint64_t i = 1; i < RADIX_SORT_STACK_SIZE; i++) {
            counts[i] += counts[i - 1];
        }

        uint64_t i = arraySize;

        // This piece of code runs 3 times faster with that optimization, as it
        // makes data more cacheable in cpu L#-cache
        for(; i > 4; i -= 4) {
            element_t<T>& element3 = elements[i - 4];
            element_t<T>& element2 = elements[i - 3];
            element_t<T>& element1 = elements[i - 2];
            element_t<T>& element0 = elements[i - 1];

            uint64_t idx3 = getKeyFrom<BYTES>(element3.key + key);
            uint64_t idx2 = getKeyFrom<BYTES>(element2.key + key);
            uint64_t idx1 = getKeyFrom<BYTES>(element1.key + key);
            uint64_t idx0 = getKeyFrom<BYTES>(element0.key + key);

            // Decrement first to get indexes which start from zero
            counts[idx0]--;
            elementBuffer[counts[idx0]] = element0;

            counts[idx1]--;
            elementBuffer[counts[idx1]] = element1;

            counts[idx2]--;
            elementBuffer[counts[idx2]] = element2;

            counts[idx3]--;
            elementBuffer[counts[idx3]] = element3;
        }

        for(; i > 0; i--) {
            uint64_t idx = getKeyFrom<BYTES>(elements[i - 1].key + key);

            // Decrement first to get indexes which start from zero
            counts[idx]--;

            elementBuffer[counts[idx]] = elements[i - 1];
        }

        // After every swap `elements` remains always sorted by some key
        std::swap(elements, elementBuffer);
    }

    for(uint64_t i = 0; i < arraySize; i++) {
        // Don't call copying constructor ( can speed up code for structs and classes )
        memcpy(out + i, begin + elements[i].index, sizeof(T));
    }

    if(freeKeys) {
        for(uint64_t i = 0; i < arraySize; i++) {
            delete[] elements[i].key;
        }
    }

    // Deleting `elements` instead of `elementsBufferOrigin` can cause a memory leak
    delete[] elementsBufferOrigin;
}

/**
 * @brief Sorts complex objects in place which are convertable to some numeric key. Has
 *        complexity of O(nw), where n is size of the array and w - count of bytes in
 *        numeric key.
 * @param begin - pointer to the array's first element
 * @param end - pointer to the element above last
 * @param out - location of allocated data to place sorted array to. Should NOT point to the
 *              same location as `begin` points to.
 * @param keyFunc - `void keyFunc(T& element, void** outkey, uint32_t* keylen)` -
 *                  type to numeric key converter. Accepts element and writes to outkey a
 *                  sequence of bytes so it becomes a n-bit key and to keylen length of that
 *                  sequence in bytes. Note that provided outkey points to a NON-ALLOCATED
 *                  data so you need to allocate it first.
 * @param freeKeys - if true, then all keys returned by keyFunc are freed by delete operator.
 * @return nothing, 
*/
template <uint32_t BYTES = 1, typename T, typename U>
void radixSortInPlace(T* begin, T* end, const U& keyFunc, bool freeKeys = false) {
    using algobox_p::element_t;

    const uint64_t RADIX_SORT_STACK_SIZE = 1ull << (BYTES << 3ull);

    uint64_t arraySize = end - begin;
    uint32_t keySize = 0;

    // *************************************************
    // *                  PREPARATION                  *
    // *************************************************

    element_t<T>* elements = new element_t<T>[arraySize * 2];
    element_t<T>* elementBuffer = elements + arraySize;

    // Save original pointer to elements buffer to be able to
    // free two buffers after changing elements and elementBuffer
    // pointers.
    element_t<T>* elementsBufferOrigin = elements;

    uint32_t currentKeySize;

    for(uint64_t i = 0; i < arraySize; i++) {
        element_t<T> &element = elements[i];

        keyFunc(begin[i], (void**)&element.key, &currentKeySize);
        keySize = std::max(keySize, currentKeySize);

        element.index = i;
    }

    uint64_t counts[RADIX_SORT_STACK_SIZE];

    // *************************************************
    // *                    SORTING                    *
    // *************************************************

    for(uint32_t key = 0; key < keySize; key += BYTES) {
        memset(counts, 0, RADIX_SORT_STACK_SIZE * sizeof(uint64_t));

        // In this implementation of radix sort values are sorted ( by digits )
        // with counting digit occurrences, going through counts array and
        // on i-th step writing back to counts[i] the sum of all `counts` elements
        // from 0 to i.
        // This allows to determine last index of each value, so decrementing
        // that index on each value occurrence we can get sorted values at the
        // end.

        for(uint64_t i = 0; i < arraySize; i++) {
            counts[getKeyFrom<BYTES>(elements[i].key + key)]++;
        }

        for(uint64_t i = 1; i < RADIX_SORT_STACK_SIZE; i++) {
            counts[i] += counts[i - 1];
        }

        uint64_t i = arraySize;

        // This piece of code runs 3 times faster with that optimization, as it
        // makes data more cacheable in cpu L#-cache
        for(; i > 4; i -= 4) {
            element_t<T>& element3 = elements[i - 4];
            element_t<T>& element2 = elements[i - 3];
            element_t<T>& element1 = elements[i - 2];
            element_t<T>& element0 = elements[i - 1];

            uint64_t idx3 = getKeyFrom<BYTES>(element3.key + key);
            uint64_t idx2 = getKeyFrom<BYTES>(element2.key + key);
            uint64_t idx1 = getKeyFrom<BYTES>(element1.key + key);
            uint64_t idx0 = getKeyFrom<BYTES>(element0.key + key);

            // Decrement first to get indexes which start from zero
            counts[idx0]--;
            elementBuffer[counts[idx0]] = element0;

            counts[idx1]--;
            elementBuffer[counts[idx1]] = element1;

            counts[idx2]--;
            elementBuffer[counts[idx2]] = element2;

            counts[idx3]--;
            elementBuffer[counts[idx3]] = element3;
        }

        for(; i > 0; i--) {
            uint64_t idx = getKeyFrom<BYTES>(elements[i - 1].key + key);

            // Decrement first to get indexes which start from zero
            counts[idx]--;

            elementBuffer[counts[idx]] = elements[i - 1];
        }

        // After every swap `elements` remains always sorted by some key
        std::swap(elements, elementBuffer);
    }

    for(uint64_t i = 0; i < arraySize; i++) {
        if(elements[i].index == (uint64_t)(-1))
            continue;

        uint64_t currentIndex = i;
        uint64_t newIndex = elements[currentIndex].index;

        T initial;

        // Copying contents through memcpy sometimes faster, at least we don't
        // call copying operator
        memcpy(&initial, begin + currentIndex, sizeof(T));

        while(newIndex != i) {
            memcpy(begin + currentIndex, begin + newIndex, sizeof(T));

            // Invalidate pointer
            elements[currentIndex].index = (uint64_t)(-1);

            currentIndex = newIndex;
            newIndex = elements[currentIndex].index;
        }
        
        elements[currentIndex].index = (uint64_t)(-1);
        memcpy(begin + currentIndex, &initial, sizeof(T));
    }

    if(freeKeys) {
        for(uint64_t i = 0; i < arraySize; i++) {
            delete[] elements[i].key;
        }
    }

    // Deleting `elements` instead of `elementsBufferOrigin` can cause a memory leak
    delete[] elementsBufferOrigin;
}

#endif