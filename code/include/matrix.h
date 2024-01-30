#pragma once
#ifndef __MATRIX_H__
    #define __MATRIX_H__
    #include <stdint.h>

// matrix for 64 bit signed integers
typedef struct i64matrix {
        size_t   nrows, ncols;
        int64_t* block;
} i64matrix_t;

// matrix for 64 bit floating points
typedef struct f64matrix {
        size_t       nrows, ncols;
        long double* block;
} f64matrix_t;

#endif //!__MATRIX_H__