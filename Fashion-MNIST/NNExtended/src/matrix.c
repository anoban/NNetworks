#define _AMD64_
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_MEAN

#include <errhandlingapi.h>
#include <handleapi.h>
#include <heapapi.h>
#include <matrix.h>
#include <stdio.h>
#include <zmmintrin.h>

matrix_t MatrixAddScalar(
    _Inout_ matrix_t matrix,
    _In_ const float scalar,
    _In_ const bool  inplace /* whether to store the results in the original buffer or to return a new matrix object */
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
    }
    const __m512 scalar_broadcast = { .m512_f32 = { scalar } }; // broadcast the scalar to an array of 16 floats
    __m512*      zmmword_0;

    const size_t nfp32szmm = sizeof(__m512) / sizeof(float);
    size_t       i         = 0;

    for (; i < matrix.dimension.nrows * matrix.dimension.ncolumns; i += nfp32szmm) {
        buffer[i] = matrix.buffer[i] / scalar;
        zmmword_0 = (__m512*) (matrix.buffer + i);
        ;
    }

    return matrix;
}

matrix_t MatrixSubScalar(_Inout_ matrix_t matrix, _In_ const float scalar, _In_ const bool inplace) { }

matrix_t MatrixMulScalar(_Inout_ matrix_t matrix, _In_ const float scalar, _In_ const bool inplace) { }

matrix_t MatrixDivScalar(_Inout_ matrix_t matrix, _In_ const float scalar, _In_ const bool inplace) { }

matrix_t MatrixMul(_In_ const matrix_t first, _In_ const matrix_t second) {
    matrix_t result = { 0 };

    // matrix multiplication can only work if the number of columns of the first matrix is equal to the number of rows of the second matrix.
    // this relation is not interchangeable, i.e cannot multiply two matrices where the number of rows of the first matrix is equal to
    // the number of columns of the second matrix
    if (first.dimension.ncolumns != second.dimension.nrows) {
        fwprintf_s(
            stderr,
            L"Error in MatrixMul, cannot multiply matrices with dimensions {%5zu, %5zu} & {%5zu, %5zu}\n",
            first.dimension.nrows,
            first.dimension.ncolumns,
            second.dimension.nrows,
            second.dimension.ncolumns
        );
        return result;
    }

    const HANDLE64 hProcHeap = GetProcessHeap(); // sidestepping malloc, will probably hurt the performance
    if (hProcHeap == INVALID_HANDLE_VALUE) {
        fputws(L"Error in MatrixMul, call to GetProcessHeap returned INVALID_HANDLE_VALUE", stderr);
        return result;
    }

    // number of elements in the product of matrix multiplication
    const size_t nreselems = first.dimension.nrows * second.dimension.ncolumns;

    float* const buffer    = HeapAlloc(hProcHeap, 0, nreselems * sizeof(float));
    if (!buffer) {
        fwprintf_s(stderr, L"Error %4lu in MatrixMul, call to HeapAlloc failed\n", GetLastError());
        return result;
    }

    // the resulting matrix will have the number of rows of the first matrix and the number of columns of the second matrix.
    const shape_t result_shape = { .nrows = first.dimension.nrows, .ncolumns = second.dimension.ncolumns };
}
