#pragma once
#ifndef __MATRIX_HPP__
    #define __MATRIX_HPP__
    #include <concepts>
    #include <utility>

    #include <iterator.hpp>
    #include <misc.hpp>

// handrolling the matrix class because I want utmost control over memory management
// if many allocations happened at every iteration of gradient descent, this won't perform any better than the python implementation
// Eigen is too messy for my taste, I cba with BLAS and it's fortran dependency and oneMKL requires about 6 GiBs of space on disk
// so yes we will be rolling our own!

template<typename T = float> requires std::is_arithmetic_v<T> class matrix final { // not interested in supporting complex types
    public:
        using value_type            = T;
        using pointer               = T*;
        using const_pointer         = const T*;
        using refernce              = T&;
        using const_refernce        = const T&;
        using size_type             = unsigned long long;
        using difference_type       = long long;
        using iterator              = random_access_iterator<T>;
        using const_iterator        = random_access_iterator<const T>;
        using column_iterator       = strided_random_access_iterator<T>;
        using const_column_iterator = strided_random_access_iterator<const T>;

    private:
        T*        _buffer;
        size_type _nrows;
        size_type _ncols;

    public:
        constexpr inline matrix() noexcept : _buffer {}, _nrows {}, _ncols {} { }

        constexpr inline matrix(_In_ const size_type& nrows, _In_ const size_type& ncols) noexcept :
            _buffer { new (std::nothrow) value_type[nrows * ncols] }, _nrows { nrows }, _ncols { ncols } {
            assert(nrows);
            assert(ncols);

            if (!_buffer) fputws(L"matrix constructor failed. matrix object is left in unusable state\n", stderr);
        }

        constexpr inline matrix(_In_ const matrix& other) noexcept :
            _buffer { new (std::nothrow) value_type[other._nrows * other._ncols] }, _nrows { other._nrows }, _ncols { other._ncols } {
            if (!_buffer) {
                fputws(L"matrix copy constructor failed. matrix object is left in unusable state\n", stderr);
                return;
            }
            std::copy(other._buffer, other._buffer + (other._nrows * other._ncols), _buffer);
        }

        constexpr inline matrix(_In_ matrix&& other) noexcept :
            _buffer { other._buffer }, _nrows { other._nrows }, _ncols { other._ncols } {
            other._buffer = nullptr;
            other._nrows = other._ncols = 0;
        }

        constexpr inline matrix& operator=(_In_ const matrix& other) noexcept {
            if (this == &other) return *this;

            return *this;
        }

        constexpr inline matrix& operator=(_In_ matrix&& other) noexcept {
            if (this == &other) return *this;

            other._buffer = nullptr;
            other._nrows = other._ncols = 0;
            return *this;
        }

        constexpr inline ~matrix() {
            delete[] _buffer;
            _nrows = _ncols = 0;
            _buffer         = nullptr;
        }

        [[nodiscard]] constexpr inline std::pair<size_type, size_type> shape() const noexcept { return { _nrows, _ncols }; }

        template<typename char_t> requires ::is_iostream_output_operator_compatible<char_t>
        friend std::basic_ostream<char_t>& operator<<(_In_ std::basic_ostream<char_t>& ostream, _In_ const matrix& matrix) { }
};

#endif // !__MATRIX_HPP__