#ifndef BINSEARCH_HPP
#define BINSEARCH_HPP
#include <stdint.h>
#include "../constants/search_priority.hpp"

template <typename T>
struct searchingResult_t {
    bool found;
    T result;
};

/**
 * Searches index of the value in the provided array. Note that for value type and for array elements type the operator<
 * and operator== must be defined.
 * @param begin - pointer to the first element
 * @param end - pointer to the element above last
 * @param value - value or key
 * @param priority - priority ( searching for the first entrance, the last or any entrance is acceptable )
 * 
 * @returns index of element. If element was not found, -1 is returned.
*/
template <typename T, typename U>
int64_t search(const T* begin, const T* end, const U& value, searchPriority priority = searchPriority::ANY_ENTRANCE) {
    const T* array = begin;

    // range is represented as [left; right) ( including left and excluding right )
    int64_t left = 0;
    int64_t right = int64_t(end - begin);

    while (left < right) {
        int64_t currentIndex = (left + right) / 2;

        // maybe theese if-branches are not pretty, but I've tried to make this
        // function work when only operator< and operator== are defined.
        if (array[currentIndex] < value) {
            left = currentIndex + 1;
        } else if (array[currentIndex] == value) {
            // Some entrance of element is found

            switch(priority) {
                case searchPriority::ANY_ENTRANCE: {
                    return currentIndex;
                }

                case searchPriority::LEFT_ENTRANCE: {
                    // Narrow down the range 'till `left` points to desired element
                    // Left and right are so chosen, where following are true:
                    // array[left] always != value
                    // array[right] always == value
                    // So, with narrowing, when left and right will differ only by
                    // one element, this will mean, that we've found left entrance.

                    // we need to keep rules above true, but somewhere in the code
                    // we could include equal element in left boundary, so exclude it.
                    left -= 1;
                    right = currentIndex;

                    while(left + 1 < right) {
                        currentIndex = (left + right) / 2;

                        if(array[currentIndex] == value) {
                            right = currentIndex;
                        } else {
                            left = currentIndex;
                        }
                    }

                    return right;
                }

                case searchPriority::RIGHT_ENTRANCE: {
                    // Narrow down the range 'till `right` points to desired element
                    // Left and right are so chosen, where following are true:
                    // array[left] always == value
                    // array[right] always != value

                    left = currentIndex;
                    // Nowhere in the code we could include equal element as right
                    // boundary, so don't exclude anything.

                    bool found = array[right] == value;

                    while(left + 1 < right) {
                        currentIndex = (left + right) / 2;

                        if(array[currentIndex] == value) {
                            left = currentIndex;
                        } else {
                            right = currentIndex;
                            found = array[right] == value;
                        }
                    }

                    return left;
                }

                // This should NEVER execute, but for safety let it be here
                default: {
                    return currentIndex;
                }
            }

        } else {
            right = currentIndex;
        }
    }

    return -1;
}

/**
 * Searches parameter which produces provided value. 
 * @param begin - left boundary of the range
 * @param end - right boundary of the range
 * @param func - function that produces desired value from searching parameter
 * @param value - value which should be produced from searching parameter
 * @param priority - priority ( searching for the first entrance, the last or any entrance is acceptable )
 * 
 * @returns desired parameter.
 *          With ANY_ENTRANCE just returns parameter, which produces desired value.
 *          With LEFT_ENTRANCE returns smallest parameter, which produces desired value.
 *          With RIGHT_ENTRANCE returns biggest parameter, which produces desired value.
*/
template <typename T, typename U>
searchingResult_t<T> paramSearch(const T& begin, const T& end, U (*func)(T x), const U& value, searchPriority priority = searchPriority::ANY_ENTRANCE) {
    // range is represented as [left; right) ( including left and excluding right )
    T left = begin;
    T right = end;

    while (left < right) {
        T currentParam = (left + right) / 2;

        // maybe theese if-branches are not pretty, but I've tried to make this
        // function work when only operator< and operator== are defined.
        if (func(currentParam) < value) {
            left = currentParam + 1;
        } else if (func(currentParam) == value) {
            // Some entrance of element is found

            switch(priority) {
                case searchPriority::ANY_ENTRANCE: {
                    return {true, currentParam};
                }

                case searchPriority::LEFT_ENTRANCE: {
                    // Narrow down the range 'till `left` points to desired element
                    // Left and right are so chosen, where following are true:
                    // array[left] always != value
                    // array[right] always == value
                    // So, with narrowing, when left and right will differ only by
                    // one element, this will mean, that we've found left entrance.

                    // we need to keep rules above true, but somewhere in the code
                    // we could include equal element in left boundary, so exclude it.
                    left -= 1;
                    right = currentParam;

                    while(left + 1 < right) {
                        currentParam = (left + right) / 2;

                        if(func(currentParam) == value) {
                            right = currentParam;
                        } else {
                            left = currentParam;
                        }
                    }

                    return {true, right};
                }

                case searchPriority::RIGHT_ENTRANCE: {
                    // Narrow down the range 'till `right` points to desired element
                    // Left and right are so chosen, where following are true:
                    // array[left] always == value
                    // array[right] always != value

                    left = currentParam;
                    // Nowhere in the code we could include equal element as right
                    // boundary, so don't exclude anything.

                    while(left + 1 < right) {
                        currentParam = (left + right) / 2;

                        if(func(currentParam) == value) {
                            left = currentParam;
                        } else {
                            right = currentParam;
                        }
                    }

                    return {true, left};
                }

                // This should NEVER execute, but for safety let it be here
                default: {
                    return {true, currentParam};
                }
            }

        } else {
            right = currentParam;
        }
    }

    return {false, (left + right) / 2};
}

#endif