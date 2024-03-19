#pragma once
#ifndef __UTILITIES_H__
    #define __UTILITIES_H__
    #include <stdlib.h>

// header only implementations

// fills the buffer with random numbers between the specified lower and upper limits.
static inline fillRandoms(_Inout_count_(size) float* const buffer, _In_ const size_t size, _In_ const float llimit, _In_ const float ulimit) {
    srand(time(NULL));

}

#endif // !__UTILITIES_H__
