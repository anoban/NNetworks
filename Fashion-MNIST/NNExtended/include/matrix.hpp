#pragma once
#ifndef __MATRIX_HPP__
    #define __MATRIX_HPP__
    #include <cstdint>

typedef struct shape {
        size_t nrows;    // number of rows in the matrix
        size_t ncolumns; // number of columns in the matrix
} shape_t;

typedef struct matrix {
        float*  buffer;    // let's settle with 32 bit floats
        shape_t dimension; // could just decompoe this into two plain uint32_t s :(
                           // but having a dedicated type for shape makes it easier to pass as arguments to factory functions
} matrix_t;

static_assert(sizeof(matrix_t) == 24, L"Potential alignment issues :: matrix_t must be 24 bytes in size!");

// perform binary operations on all elements of the matrix.
matrix_t __stdcall MatrixAddScalar(
    _Inout_ matrix_t matrix,
    _In_ const float scalar,
    _In_ const bool  inplace /* whether to store the results in the original buffer or to return a new matrix object */
);

matrix_t __stdcall MatrixSubScalar(_Inout_ matrix_t matrix, _In_ const float scalar, _In_ const bool inplace);

matrix_t __stdcall MatrixMulScalar(_Inout_ matrix_t matrix, _In_ const float scalar, _In_ const bool inplace);

matrix_t __stdcall MatrixDivScalar(_Inout_ matrix_t matrix, _In_ const float scalar, _In_ const bool inplace);

// matrix - matrix multiplication.
matrix_t __stdcall MatrixMul(_In_ const matrix_t first, _In_ const matrix_t second);

#endif // !__MATRIX_HPP__
