#pragma once
#if defined __clang__ || defined __llvm__
    #error "matrix.c uses MSVC AVX512 intrinsics, incompatible with LLVM intrinsics!"
#endif // !__clang__ || defined __llvm__

#ifndef __MATRIX_H__
    #define __MATRIX_H__
    #include <stdbool.h>
    #include <stdint.h>

typedef struct shape {
        size_t nrows;    // number of rows in the matrix
        size_t ncolumns; // number of columns in the matrix
} shape_t;

typedef struct matrix {
        float*  buffer; // let's settle with 32 bit floats
        shape_t dimension;
} matrix_t;

// perform binary operations on all elements of the matrix.
matrix_t MatrixAddScalar(
    _Inout_ matrix_t matrix,
    _In_ const float scalar,
    _In_ const bool  inplace /* whether to store the results in the original buffer or to return a new matrix object */
);

matrix_t MatrixSubScalar(_Inout_ matrix_t matrix, _In_ const float scalar, _In_ const bool inplace);

matrix_t MatrixMulScalar(_Inout_ matrix_t matrix, _In_ const float scalar, _In_ const bool inplace);

matrix_t MatrixDivScalar(_Inout_ matrix_t matrix, _In_ const float scalar, _In_ const bool inplace);

// matrix - matrix multiplication.
matrix_t MatrixMul(_In_ const matrix_t first, _In_ const matrix_t second);

#endif // !__MATRIX_H__
