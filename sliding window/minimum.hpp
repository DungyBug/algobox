#include <vector>
#include <queue>
#include <stdint.h>

template <typename T>
std::vector<T> minimumForSlidingWindow(const std::vector<T> &array, uint32_t slidingWindowWidth) {
    if(slidingWindowWidth >= array.size()) {
        // Sliding window covers whole array, so we have only one minimum.
        // Do that check for convenient function use.

        T min = array[0];

        for(const T& value : array) {
            if(value < min)
                min = value;
        }

        std::vector<T> out(1);

        out[0] = min;

        return out;
    }

    std::vector<T> minimums(array.size() - slidingWindowWidth + 1);

    std::deque<T> minimum;

    // Fill up queue with first window

    for(uint32_t i = 0; i < slidingWindowWidth; i++) {
        while(!minimum.empty() && minimum.back() > array[i])
            minimum.pop_back();

        minimum.push_back(array[i]);
    }

    minimums[0] = minimum.front();

    // Current index in minimums array
    uint32_t currentIdx = 1;

    for(uint32_t i = slidingWindowWidth; i < array.size(); i++) {
        if(minimum.front() == array[i - slidingWindowWidth]) {
            minimum.pop_front();
        }

        while(!minimum.empty() && minimum.back() > array[i])
            minimum.pop_back();

        minimum.push_back(array[i]);

        minimums[currentIdx] = minimum.front();
            
        currentIdx++;
    }

    return minimums;
}