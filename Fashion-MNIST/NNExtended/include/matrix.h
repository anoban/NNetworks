#pragma once
#ifndef __MATRIX_H__
    #define __MATRIX_H__
    #include <stdbool.h>
    #include <stdint.h>

// enum to specify the type of binary operation between the matrix and a scalar (float)
typedef enum MATRIX_SCALAR_OPERATION { ADDITION = 0x0A0B0C, SUBTRACTION, DIVISION, MULTIPLICATION } MATRIX_SCALAR_OPERATION_KIND;

typedef struct shape {
        size_t nrows;    // number of rows in the matrix
        size_t ncolumns; // number of columns in the matrix
} shape_t;

typedef struct matrix {
        float*  buffer; // let's settle with 32 bit floats
        shape_t dimension;
} matrix_t;

// perform binary operations on all elements of the matrix.
matrix_t MatrixOpScalar(
    _Inout_ matrix_t                        matrix,
    _In_ const float                        scalar,
    _In_ const MATRIX_SCALAR_OPERATION_KIND operation,
    _In_ const bool inplace /* whether to store the results in the original buffer or to return a new matrix object */
);

// matrix - matrix multiplication.
matrix_t MatrixMul(_In_ const matrix_t first, _In_ const matrix_t second);

#endif // !__MATRIX_H__
