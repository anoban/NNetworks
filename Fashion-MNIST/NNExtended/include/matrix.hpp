#pragma once
#ifndef __MATRIX_HPP__
    #define __MATRIX_HPP__
    #include <concepts>
    #include <utility>

    #include <iterator.hpp>
    #include <misc.hpp>

// handrolling the matrix class because I want more granular control over memory management
// if many allocations happened at every iteration of gradient descent, this won't perform any better than the python implementation
// and Eigen is too messy for my taste, I cba with BLAS and it's fortran dependency and oneMKL requires about 6 GiBs of space on disk so yes we will be rolling our own!

template<typename T = float> requires std::is_arithmetic_v<T> class matrix final { // not interested in supporting complex types
    public:
        using value_type      = T;
        using pointer         = T*;
        using const_pointer   = const T*;
        using refernce        = T&;
        using const_refernce  = const T&;
        using size_type       = unsigned long long;
        using difference_type = long long;
        using iterator        = random_access_iterator<T>;
        using const_iterator  = random_access_iterator<const T>;

    private:
        T*        _buffer;
        size_type _nrows;
        size_type _ncols;

    public:
        constexpr inline matrix() noexcept : _buffer {}, _nrows {}, _ncols {} { }

        inline matrix(_In_ const size_type& nrows, _In_ const size_type& ncols) noexcept :
            _buffer { new (std::nothrow) value_type[nrows * ncols] }, _nrows { nrows }, _ncols { ncols } {
            if (!_buffer)
                ;
        }

        [[nodiscard]] constexpr inline std::pair<size_type, size_type> shape() const noexcept { }

        template<typename char_t> requires ::is_iostream_output_operator_compatible<char_t>
        friend std::basic_ostream<char_t>& operator<<(_In_ std::basic_ostream<char_t>& ostream, _In_ const matrix& matrix) { }
};

#endif // !__MATRIX_HPP__