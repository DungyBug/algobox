#include <algorithm>
#include <vector>
#include <string>
#include <stdint.h>
#include "../../sort/radix/radix.hpp"

namespace algobox_p {

struct mark_t {
    union {
        struct {
            uint32_t b;
            uint32_t a;
        };

        // Contains b and a in one integer. Gives big speed up when sorting
        uint64_t ba = 0;
    };
    uint64_t index = 0;
};

bool operator<(const mark_t &left, const mark_t &right) {
    return left.ba < right.ba;
};

};

std::vector<uint32_t> buildSuffixArray(const std::string &str) {
    using namespace algobox_p;

    const uint32_t size = str.size();

    std::vector<uint32_t> suffixArray(size, 0);

    // This keeps pointers to mark of corresponding substring.
    // So, marks[indices[4]] will be mark for substring [4..n]
    std::vector<uint32_t> indices(size, 0);

    std::vector<mark_t> marks(size);

    for(uint32_t i = 0; i < size; i++) {
        marks[i].index = i;
        marks[i].a = str[i];
    }

    for(uint32_t k = 1; k < size; k *= 2) {
        std::sort(marks.begin(), marks.end());

        uint32_t currentRank = 0;
        uint64_t prevBA = -1;

        for(uint32_t i = 0; i < size; i++) {
            indices[marks[i].index] = i;

            if(marks[i].ba != prevBA) {
                prevBA = marks[i].ba;
                currentRank++;
            }

            marks[i].a = currentRank;
        }

        for(uint32_t i = 0; i < size; i++) {
            if (marks[i].index + k < size) {
                marks[i].b = marks[indices[marks[i].index + k]].a;
            } else {
                marks[i].b = 0;
            }
        }
    }

    std::sort(marks.begin(), marks.end());

    for(uint32_t i = 0; i < size; i++) {
        indices[i] = marks[i].index;
    }

    return indices;
}