#pragma once
#include <concepts>
#include <utility>

#include <iterator.hpp>
#include <utilities.hpp>

// handrolling the matrix class because we want utmost control over memory management
// if many allocations happened at every iteration of gradient descent, this won't perform any better than the python implementation
// Eigen is too messy for my taste, cba with BLAS and it's fortran dependencies and oneMKL requires about 6 GiBs of space on disk
// so yes we will be rolling our own!

template<typename _Ty> requires std::floating_point<_Ty> class matrix final {
    public:
        using value_type            = _Ty; // can be float, double or long double
        using pointer               = _Ty*;
        using const_pointer         = const _Ty*;
        using refernce              = _Ty&;
        using const_refernce        = const _Ty&;
        using size_type             = unsigned long long;
        using difference_type       = long long;
        using iterator              = random_access_iterator<_Ty>;
        using const_iterator        = random_access_iterator<const _Ty>;
        using column_iterator       = strided_random_access_iterator<_Ty>;
        using const_column_iterator = strided_random_access_iterator<const _Ty>;

        // clang-format off
    #ifndef __TEST__ // if __TEST__ is defined, make these private member variables public, for testing
    private:
    #endif //
        // clang-format on
        _Ty*      _buffer; // internal buffer
        size_type _nrows;
        size_type _ncols;

        inline void __stdcall __cleanup() noexcept { // a convenient private routine to do self cleanups
            delete[] _buffer;
            _nrows = _ncols = 0;
        }

        inline void __stdcall __cleanup(_Inout_ matrix& _matrix) noexcept { // a convenient private routine to cleanup moved from matrices
            _matrix._buffer = nullptr;
            _matrix._nrows = _matrix._ncols = 0;
        }

    public:
        inline matrix() = delete; // will an empty matrix ever be useful?

        // construct an empty matrix with specified dimensions
        inline matrix(_In_ const size_type& nrows, _In_ const size_type& ncols) noexcept :
            _buffer { new (std::nothrow) value_type[nrows * ncols] }, _nrows { nrows }, _ncols { ncols } {
            assert(nrows);
            assert(ncols);
            if (!_buffer) {
                _nrows = _ncols = 0;
                fputws(L"memory allocation failed inisde " __FUNCSIG__ ". matrix object was default initialized as a fallback\n", stderr);
            }
        }

        inline matrix(_In_ const matrix& other) noexcept :
            _buffer { new (std::nothrow) value_type[other._nrows * other._ncols] }, _nrows { other._nrows }, _ncols { other._ncols } {
            if (!_buffer) {
                _nrows = _ncols = 0;
                fputws(L"memory allocation failed inisde " __FUNCSIG__ ". matrix object was default initialized as a fallback\n", stderr);
                return;
            }
            std::copy(other._buffer, other._buffer + (other._nrows * other._ncols), _buffer);
        }

        inline matrix(_In_ matrix&& other) noexcept : _buffer { other._buffer }, _nrows { other._nrows }, _ncols { other._ncols } {
            __cleanup(other);
        }

        inline matrix& operator=(_In_ const matrix& other) noexcept {
            if (this == &other) return *this;

            return *this;
        }

        inline matrix& operator=(_In_ matrix&& other) noexcept {
            if (this == &other) return *this;

            _buffer = other._buffer;
            _nrows  = other._nrows;
            _ncols  = other._ncols;

            __cleanup(other);
            return *this;
        }

        inline ~matrix() noexcept { __cleanup(); }

        inline size_type rows() const noexcept { return _nrows; }

        inline size_type columns() const noexcept { return _ncols; }

        template<typename char_t> requires ::is_iostream_output_operator_compatible<char_t>
        friend std::basic_ostream<char_t>& operator<<(_In_ std::basic_ostream<char_t>& ostream, _In_ const matrix& matrix) {
            // TODO
        }

        inline const_iterator cbegin() const noexcept { return { _buffer /* resource pointer */, _nrows * _ncols /* length */ }; }

        inline const_iterator cend() const noexcept {
            return { _buffer /* resource pointer */, _nrows * _ncols /* length */, _nrows * _ncols /* offset */ };
        }
};
