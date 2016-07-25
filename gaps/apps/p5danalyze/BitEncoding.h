#ifndef BIT_ENC
#define BIT_ENC

#include <vector>
#include <math.h>

// This is the rough layout of the bitencoding code

class BitEncoding {
    public:
        BitEncoding(int options);
        int GetRasterizationValue(int);
        std::vector<bool> Unencode(int);

    private:
        int n_options;
};

BitEncoding::BitEncoding(int options) {
    n_options = options;
}

// Give the enum you wish to encode, returns the value to add
int BitEncoding::
GetRasterizationValue(int i) {
    return int(pow(2, i));
}

std::vector<bool> BitEncoding::
Unencode(int value) {
    std::vector<bool> results(n_options);
    for (int i = n_options - 1; i >= 0; i--) {
        int divisor = int(pow(2, i));
        if (value >= divisor) {
            results[i] = true;
            value -= divisor;
        }
    }

    return results;
}

#endif
