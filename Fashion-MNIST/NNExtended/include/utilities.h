#pragma once
#ifndef __UTILITIES_H__
    #define __UTILITIES_H__
    #include <matrix.h>
    #include <stdlib.h>
    #include <time.h>

// fills the buffer with random numbers between the specified lower and upper limits.
static inline void FillRandoms(_In_ matrix_t matrix, _In_ const float llimit, _In_ const float ulimit) {
    srand(time(NULL));
    if (!matrix.buffer) { }
    for (size_t i = 0; i < matrix.dimension.nrows * matrix.dimension.ncolumns; ++i) matrix.buffer[i] = rand() / RAND_MAX;
    return;
}

static inline void ReLU() { }

static inline void LeakyReLU() { }

static inline void SoftMax() { }

#endif // !__UTILITIES_H__
