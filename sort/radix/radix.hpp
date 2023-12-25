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
    T* element;
};

};

/**
 * @brief Sorts complex objects which are convertable to some numeric key. Has complexity of
 *        O(nw), where n is size of the array and w - count of bytes in numeric key.
 * @param begin - pointer to the array's first element
 * @param end - pointer to the element above last
 * @param out - location of allocated data to place sorted array to. Should NOT point to the
 *              same location as `begin` points to.
 * @param keyFunc - type to numeric key converter. Accepts element and writes to outkey a
 *                  sequence of bytes so it becomes a n-bit key and to keylen length of that
 *                  sequence in bytes. Note that provided outkey points to a NON-ALLOCATED
 *                  data so you need to allocate it first.
 * @param freeKeys - if true, then all keys returned by keyFunc are freed by delete operator.
 * @return nothing, 
*/
template <typename T>
void radixSort(T* begin, T* end, T* out, const std::function<void(T& element, void** outkey, uint32_t* keylen)> &keyFunc, bool freeKeys = false) {
    using algobox_p::element_t;

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

        element.element = begin + i;
    }

    uint64_t counts[256];

    // *************************************************
    // *                    SORTING                    *
    // *************************************************

    for(uint32_t key = 0; key < keySize; key++) {
        memset(counts, 0, 256 * sizeof(uint64_t));

        // In this implementation of radix sort values are sorted ( by digits )
        // with counting digit occurrences, going through counts array and
        // on i-th step writing back to counts[i] the sum of all `counts` elements
        // from 0 to i.
        // This allows to determine last index of each value, so decrementing
        // that index on each value occurrence we can get sorted values at the
        // end.

        for(uint64_t i = 0; i < arraySize; i++) {
            counts[elements[i].key[key]]++;
        }

        for(uint64_t i = 1; i < 256; i++) {
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

            uint64_t idx3 = element3.key[key];
            uint64_t idx2 = element2.key[key];
            uint64_t idx1 = element1.key[key];
            uint64_t idx0 = element0.key[key];

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
            uint64_t idx = elements[i - 1].key[key];

            // Decrement first to get indexes which start from zero
            counts[idx]--;

            elementBuffer[counts[idx]] = elements[i - 1];
        }

        // After every swap `elements` remains always sorted by some key
        std::swap(elements, elementBuffer);
    }

    for(uint64_t i = 0; i < arraySize; i++) {
        out[i] = *elements[i].element;
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
 * @brief Sorts complex objects which are convertable to some numeric key. Has complexity of
 *        O(nw), where n is size of the array and w - count of bytes in numeric key.
 * @param begin - pointer to the array's first element
 * @param end - pointer to the element above last
 * @param out - location of allocated data to place sorted array to. Should NOT point to the
 *              same location as `begin` points to.
 * @param keyFunc - type to numeric key converter. Accepts element and writes to outkey a
 *                  sequence of bytes so it becomes a n-bit key and to keylen length of that
 *                  sequence in bytes. Note that provided outkey points to a NON-ALLOCATED
 *                  data so you need to allocate it first.
 * @param freeKeys - if true, then all keys returned by keyFunc are freed by delete operator.
 * @return nothing, 
*/
template <typename T>
void radixSort(T* begin, T* end, T* out, void (*keyFunc)(T& element, void** outkey, uint32_t* keylen), bool freeKeys = false) {
    using algobox_p::element_t;

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

        element.element = begin + i;
    }

    uint64_t counts[0x100];

    // *************************************************
    // *                    SORTING                    *
    // *************************************************

    for(uint32_t key = 0; key < keySize; key++) {
        memset(counts, 0, 0x100 * sizeof(uint64_t));

        // In this implementation of radix sort values are sorted ( by digits )
        // with counting digit occurrences, going through counts array and
        // on i-th step writing back to counts[i] the sum of all `counts` elements
        // from 0 to i.
        // This allows to determine last index of each value, so decrementing
        // that index on each value occurrence we can get sorted values at the
        // end.

        for(uint64_t i = 0; i < arraySize; i++) {
            counts[elements[i].key[key]]++;
        }

        if(counts[0] == arraySize)
            break;

        for(uint64_t i = 1; i < 0x100; i++) {
            counts[i] += counts[i - 1];
        }

        uint64_t i = arraySize;

        for(; i > 4; i -= 4) {
            element_t<T>& element3 = elements[i - 4];
            element_t<T>& element2 = elements[i - 3];
            element_t<T>& element1 = elements[i - 2];
            element_t<T>& element0 = elements[i - 1];

            uint64_t idx3 = element3.key[key];
            uint64_t idx2 = element2.key[key];
            uint64_t idx1 = element1.key[key];
            uint64_t idx0 = element0.key[key];

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
            uint64_t idx = elements[i - 1].key[key];

            // Decrement first to get indexes which start from zero
            counts[idx]--;

            elementBuffer[counts[idx]] = elements[i - 1];
        }

        // After every swap `elements` remains always sorted by some key
        std::swap(elements, elementBuffer);
    }

    for(uint64_t i = 0; i < arraySize; i++) {
        out[i] = *elements[i].element;
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