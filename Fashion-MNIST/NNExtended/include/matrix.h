#pragma once
#ifndef __MATRIX_H__
    #define __MATRIX_H__
    #include <stdbool.h>
    #include <stdint.h>

typedef enum MATRIX_SCALAR_OPERATION { ADD = 0x0A0B0C, SUBTRACT, DIVIDE, MULTIPLY } MATRIX_SCALAR_OPERATION_KIND;

typedef struct matrix {
        float* buffer; // let's settle with 32 bit floats
        size_t nrows;  // number of rows in the matrix
        size_t ncols;  // number of columns in the matrix
} matrix_t;

matrix_t MatrixOpScalar(_In_ matrix_t matrix, _In_ const MATRIX_SCALAR_OPERATION_KIND operation, _In_ const bool inplace);

matrix_t MatrixMul(_In_ const matrix_t first, _In_ const matrix_t second);

#endif // !__MATRIX_H__
