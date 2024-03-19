#define _AMD64_
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_MEAN

#include <errhandlingapi.h>
#include <handleapi.h>
#include <heapapi.h>
#include <matrix.h>

matrix_t MatrixOpScalar(
    _Inout_ matrix_t                        matrix,
    _In_ const float                        scalar,
    _In_ const MATRIX_SCALAR_OPERATION_KIND operation,
    _In_ const bool inplace /* whether to store the results in the original buffer or to return a new matrix object */
) {
    matrix_t tmp    = { 0 };
    float*   buffer = NULL; // in case inplace = false

    if (!inplace) {
        const HANDLE64 hProcHeap = GetProcessHeap();
        if (hProcHeap == INVALID_HANDLE_VALUE) {
            ;
            return tmp;
        }
        if (!(buffer = HeapAlloc(hProcHeap, 0, matrix.dimension.nrows * matrix.dimension.ncolumns * sizeof(float)))) {
            ;
            return tmp;
        }
    } else
        buffer = matrix.buffer; // aliasing

    switch (operation) {
        case ADDITION :
            for (size_t i = 0; i < matrix.dimension.nrows * matrix.dimension.ncolumns; ++i) buffer[i] = matrix.buffer[i] + scalar;
            break;
        case SUBTRACTION :
            for (size_t i = 0; i < matrix.dimension.nrows * matrix.dimension.ncolumns; ++i) buffer[i] = matrix.buffer[i] - scalar;
            break;
        case MULTIPLICATION :
            for (size_t i = 0; i < matrix.dimension.nrows * matrix.dimension.ncolumns; ++i) buffer[i] = matrix.buffer[i] * scalar;
            break;
        case DIVISION :
            for (size_t i = 0; i < matrix.dimension.nrows * matrix.dimension.ncolumns; ++i) buffer[i] = matrix.buffer[i] / scalar;
            break;
        default : break;
    }
    return matrix;
}

matrix_t MatrixMul(_In_ const matrix_t first, _In_ const matrix_t second) {
    matrix_t      tmp          = { 0 };
    float* const  buffer       = NULL; // in case inplace = false
    const shape_t result_shape = { .nrows = 0, .ncolumns = 0 };
}
