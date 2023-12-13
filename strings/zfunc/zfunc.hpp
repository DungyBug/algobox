#ifndef ZFUNC_HPP
#define ZFUNC_HPP
#include <vector>
#include <string>
#include <stdint.h>

inline std::vector<uint64_t> zfunc(const std::string &str) {
    uint64_t size = str.size();
    const char* input = str.c_str();

    std::vector<uint64_t> zvalues(size, 0);

    uint64_t l = 0, r = 0;

    for(uint64_t i = 1; i < size; i++) {
#if 0
        uint64_t currentLen = zvalues[i - l];

        if(i + currentLen > r) {
            currentLen = r > i ? r - i : 0;

            while(input[i + currentLen] == input[currentLen])
                currentLen++;

            l = i;
            r = i + currentLen - 1;
        }

        zvalues[i] = currentLen;
#else
        // Making temp variable instead of modifying zvalues[i] directly
        // significantly increases perfomance, making zfunc ~40% faster
        // with -O0 flag.
        // Honestly, I don't know why does compiler don't do it for me...
        uint64_t currentLen = 0;

        if(r < i) {
            // Original algorithm has additional check for `i + currentLen < size`,
            // but as strings are null-terminated and i + currentLen always bigger
            // by at least one than currentLen, we can skip this check.
            // After all, input[i + currentLen] at some point becomes NULL-character
            // and, obviosly, won't be equal to any of the string characters.
            // At least, none of the million tests marks this as buggy code.
            while(input[i + currentLen] == input[currentLen])
                currentLen++;

            l = i;
            r = i + currentLen - 1;
        } else if(i + zvalues[i - l] <= r) {
            currentLen = zvalues[i - l];
        } else {
            currentLen = r - i + 1;

            while(input[i + currentLen] == input[currentLen])
                currentLen++;

            l = i;
            r = i + currentLen - 1;
        }

        zvalues[i] = currentLen;
#endif
    }

    return zvalues;
};

#endif