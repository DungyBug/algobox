#ifndef ZFUNC_HPP
#define ZFUNC_HPP
#include <vector>
#include <string>
#include <stdint.h>

std::vector<uint64_t> zfunc(const std::string &str) {
    uint64_t size = str.size();
    std::vector<uint64_t> zvalues(size, 0);

    uint64_t l = 0, r = 0;

    for(uint64_t i = 1; i < size; i++) {
#if 0
        // This is shorter, but slower. Benched with -O0 and -O1 flags.
        // I'll inspect this part, maybe I'll make it faster
        uint64_t currentLen = 0;

        if(i <= r)
            currentLen = std::min(zvalues[i - l] + i, r) - i;

        while(i + currentLen < size && str[i + currentLen] == str[currentLen])
            currentLen++;

        if(i + zvalues[i - l] > r) {
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
            while(i + currentLen < size && str[i + currentLen] == str[currentLen])
                currentLen++;

            l = i;
            r = i + currentLen - 1;
        } else if(i + zvalues[i - l] <= r) {
            currentLen = zvalues[i - l];
        } else {
            currentLen = r - i;

            while(i + currentLen < size && str[i + currentLen] == str[currentLen])
                currentLen++;

            l = i;
            r = i + currentLen - 1;
        }

        zvalues[i] = currentLen;
#endif
    }

    return zvalues;
}

#endif