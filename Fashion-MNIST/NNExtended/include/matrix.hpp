#pragma once
#ifndef __MATRIX_HPP__
    #define __MATRIX_HPP__
    #include <cstdint>
    #include <type_traits>

template<typename T, typename = typename std::enable_if<std::is_arithmetic_v<T>, T>::type> class matrix {
    public:
        using value_type      = std::remove_cv_t<T>;
        using pointer         = value_type*;
        using const_pointer   = const value_type*;
        using reference       = value_type&;
        using const_reference = const value_type&;
        using difference_type = ptrdiff_t;
        using size_type       = unsigned long long;

    private:
        size_type   _nrows; // number of rows in the matrix
        size_type   _ncols; // number of columns in the matrix
        value_type* _rsrc;
};

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
