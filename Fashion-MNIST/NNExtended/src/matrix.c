#define _AMD64_
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_MEAN

#include <errhandlingapi.h>
#include <handleapi.h>
#include <heapapi.h>
#include <matrix.h>

matrix_t MatrixOpScalar(_In_ matrix_t matrix, _In_ const MATRIX_SCALAR_OPERATION_KIND operation, _In_ const bool inplace) {
    matrix_t tmp    = { 0 };
    float*   buffer = NULL; // in case inplace = false

    if (!inplace) {
        const HANDLE64 hProcHeap = GetProcessHeap();
        buffer                   = HeapAlloc(hProcHeap, 0, matrix.dimension.nrows * matrix.dimension.ncolumns * sizeof(float));
        ;
    } else
        buffer = matrix.buffer; // aliasing
}
matrix_t MatrixMul(_In_ const matrix_t first, _In_ const matrix_t second) {
    matrix_t      tmp          = { 0 };
    float* const  buffer       = NULL; // in case inplace = false
    const shape_t result_shape = { .nrows = 0, .ncolumns = 0 };
}
