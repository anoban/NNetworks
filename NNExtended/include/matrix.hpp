#pragma once
#include <immintrin.h>

#include <iterator.hpp>
#include <utilities.hpp>

// handrolling the matrix class because we want utmost control over memory management
// if many allocations happened at every iteration of gradient descent, this won't perform any better than the python implementation
// Eigen is too messy for my taste, cba with BLAS and it's fortran dependencies and oneMKL requires about 6 GiBs of space on disk
// so yes we will be rolling our own!

class matrix final {
    private:
        using value_type            = float;
        using pointer               = value_type*;
        using const_pointer         = const value_type*;
        using refernce              = value_type&;
        using const_refernce        = const value_type&;
        using size_type             = unsigned long long;
        using difference_type       = long long;
        using iterator              = random_access_iterator<value_type>;
        using const_iterator        = random_access_iterator<const value_type>;
        using column_iterator       = strided_random_access_iterator<value_type>;
        using const_column_iterator = strided_random_access_iterator<const value_type>;

        // clang-format off
    #ifdef __TEST__ // if __TEST__ is defined, make these private member variables public for testing
    public:
    #endif //
        // clang-format on
        pointer   _buffer; // internal buffer
        size_type _nrows;
        size_type _ncols;

        void __stdcall __cleanup() noexcept { // a convenient private routine to do self cleanups
            delete[] _buffer;
            _nrows = _ncols = 0;
        }

        static void __stdcall __cleanup(_Inout_ matrix& _matrix) noexcept { // a convenient private routine to cleanup moved from matrices
            _matrix._buffer = nullptr;
            _matrix._nrows = _matrix._ncols = 0;
        }

    public:
        matrix() = delete; // will an empty matrix ever be useful?

        // construct an empty matrix with specified dimensions
        matrix(_In_ const size_type& nrows, _In_ const size_type& ncols) noexcept :
            _buffer { new (std::nothrow) value_type[nrows * ncols] }, _nrows { nrows }, _ncols { ncols } {
            assert(nrows);
            assert(ncols);
            if (!_buffer) {
                _nrows = _ncols = 0;
                fputws(L"memory allocation failed inisde " __FUNCSIG__ ". matrix object was default initialized as a fallback\n", stderr);
            }
        }

        matrix(_In_ const matrix& other) noexcept :
            _buffer { new (std::nothrow) value_type[other._nrows * other._ncols] }, _nrows { other._nrows }, _ncols { other._ncols } {
            if (!_buffer) {
                _nrows = _ncols = 0;
                fputws(L"memory allocation failed inisde " __FUNCSIG__ ". matrix object was default initialized as a fallback\n", stderr);
                return;
            }
            std::copy(other._buffer, other._buffer + (other._nrows * other._ncols), _buffer);
        }

        matrix(_In_ matrix&& other) noexcept : _buffer { other._buffer }, _nrows { other._nrows }, _ncols { other._ncols } {
            __cleanup(other);
        }

        matrix& operator=(_In_ const matrix& other) noexcept {
            if (this == &other) return *this;

            return *this;
        }

        matrix& operator=(_In_ matrix&& other) noexcept {
            if (this == &other) return *this;

            _buffer = other._buffer;
            _nrows  = other._nrows;
            _ncols  = other._ncols;

            __cleanup(other);
            return *this;
        }

        ~matrix() noexcept { __cleanup(); }

        size_type rows() const noexcept { return _nrows; }

        size_type columns() const noexcept { return _ncols; }

        template<typename char_t>
        friend typename std::enable_if<::is_iostream_output_operator_compatible<char_t>, std::basic_ostream<char_t>&>::type operator<<(
            _In_ std::basic_ostream<char_t>& ostream, _In_ const matrix& matrix
        ) noexcept {
            // TODO
        }

        const_iterator cbegin() const noexcept { return { _buffer /* resource pointer */, _nrows * _ncols /* length */ }; }

        const_iterator cend() const noexcept {
            return { _buffer /* resource pointer */, _nrows * _ncols /* length */, _nrows * _ncols /* offset */ };
        }

        pointer data() noexcept { return _buffer; }

        const_pointer data() const noexcept { return _buffer; }

        // choosing to implement matrix multiplication as a non-member function instead of overloading operator*() or
        // operator*=() because these approaches will compromise on performance
        // in a sitiation where an expression like this matrix = matrix_0 * matrix_1 gets iteratively executed,
        // we'll pay a lot for the gratuitous memory allocations for the matrix multiplication product
        // providing a custom move assignment operator cannot help us prevent the allocation needed to store the product before it is returned by operator whatnot()
        // this is acceptable if these operators get called once or twice, but during gradient descent, these kind of expressions will be evaluated
        // tens of thousands of times iteratively, where the dimensions of the matrices do not change!
        // by using a friend function, we can use a third argument as an in-out parameter where the multiplication product can be
        // materialized, so once all the matrices have been allocated with memory, we could reuse the same buffers throughout all the
        // iterations
        friend bool dotprod(_In_ const matrix& left, _In_ const matrix& right, _Inout_ matrix& prod) noexcept {
            // VALIDATE THE PRECONDITIONS

            // number of columns in the left matrix must be equal to the number of rows in the right matrix
            if (left._ncols != right._nrows) { // cannot perform a matrix multiplication without this
                ::fwprintf_s(stderr, L"");
                return false;
            }

            // because we ain't that stupid
            // if the result does not have enough memory to store the dot prduct, do a reallocation
            // hopefully this newly allocated buffer can be reused throughout all the iterations
            // the dot product will have the same number of rows as the left matrix and same number of columns as the right matrix
            if (left._nrows * right._ncols > prod._nrows * prod._ncols) { // perform a reallocation
                delete[] prod._buffer;                                    // lose the old buffer, if one existed
                prod._buffer = new (std::nothrow) matrix::value_type[left._nrows * right._ncols];

                if (!prod._buffer) {
                    ::fwprintf_s(stderr, L"");
                    prod._buffer = nullptr;
                    prod._nrows = prod._ncols = 0;
                    return false;
                }

                prod._nrows = left._nrows;
                prod._ncols = right._ncols;
            }

            // NOW TO THE ACTUAL MATRIX MULTIPLICATION

            // number of value_types that can be packed in a 512 bits wide zmm register
            const size_t zmm_packable_valtype_count { sizeof(__m512) / sizeof(matrix::value_type) };

            for (size_t i = 0; i < left._nrows /* number of rows in the dotproduct */; ++i) {
                //
            }
        }
};
