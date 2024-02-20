#ifndef BINPOW_HPP
#define BINPOW_HPP

template <typename T, typename U>
T binpow(T x, U power) {
    if (power == 1) {
        return x;
    }

    T out = T(1);

    // Usual binpow that goes through binary represenation of power

    while (power > 0) {
        // For some reason, adding an "else" branch makes this code
        // faster, despite else branch is guaranteed to execute in the
        // next cycle ( so, logically, we should loose cycles rather than gain
        // them )
        if (power % 2) {
            out *= x;
            power--;
        } else {
            x *= x;
            power /= 2;
        }
    }

    return out;
}

#endif