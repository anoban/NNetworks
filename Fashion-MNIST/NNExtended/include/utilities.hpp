#pragma once
#ifndef __UTILITIES_HPP__
    #define __UTILITIES_HPP__
    #include <cstdlib>
    #include <iterator>
    #include <random>

namespace utilities {

    // fills the buffer with random numbers between -0.5 and 0.5
    static inline void __stdcall FillRandoms(_In_ matrix_t matrix) {
        srand(time(NULL));
        if (!matrix.buffer) { }
        // rand() will return values between 0 and RAND_MAX, so rand() / RAND_MAX will normalize these values between 0 and 1
        // subtracting 0.5 will shift them between -0.5 to .5
        for (size_t i = 0; i < matrix.dimension.nrows * matrix.dimension.ncolumns; ++i)
            matrix.buffer[i] = (rand() / (float) RAND_MAX) - 0.50000F;
        return;
    }

    static inline void ReLU() { }

    static inline void LeakyReLU() { }

    static inline void SoftMax() { }

} // namespace utilities

#endif // !__UTILITIES_HPP__