#ifndef BINPOW_HPP
#define BINPOW_HPP

template <typename T, typename U>
T binpow(const T& x, U power) {
    if (power == 1) {
        return x;
    }

    power--;

    U currentPower = 1;
    T powered = x;
    T out = x;

    while (power > 0) {
        if (power & 1) {
            out *= powered;
        }

        powered *= powered;
        power >>= 1;
    }

    return out;
}

#endif