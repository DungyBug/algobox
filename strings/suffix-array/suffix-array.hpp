#include <algorithm>
#include <vector>
#include <string>
#include <stdint.h>

struct mark_t {
    uint32_t a = 0, b = 0, index = 0;
};

bool operator<(const mark_t &left, const mark_t &right) {
    return left.a < right.a || (left.a == right.a && left.b < right.b);
};

std::vector<uint32_t> buildSuffixArray(const std::string &str) {
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
        uint32_t prevA = -1;
        uint32_t prevB = -1;

        for(uint32_t i = 0; i < size; i++) {
            indices[marks[i].index] = i;

            if(marks[i].a != prevA || marks[i].b != prevB) {
                prevA = marks[i].a;
                prevB = marks[i].b;
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