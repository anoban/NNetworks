#pragma once
#ifndef __UTILITIES_H__
    #define __UTILITIES_H__
    #include <stdlib.h>
    #include <time.h>

// header only implementations

// fills the buffer with random numbers between the specified lower and upper limits.
static inline void fillRandoms(
    _Inout_count_(size) float* const buffer, _In_ const size_t size, _In_ const float llimit, _In_ const float ulimit
) {
    srand(time(NULL));
}

static inline void ReLU() { }

static inline void leakyReLU() { }

static inline void softMax() { }

#endif // !__UTILITIES_H__
