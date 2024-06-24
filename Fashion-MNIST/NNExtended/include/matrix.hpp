#pragma once
#ifndef __MATRIX_HPP__
    #define __MATRIX_HPP__
    #include <concepts>

    #include <iterator.hpp>

// handrolling the matrix class because I want more granular control over memory management
// if many allocations happened at every iteration of gradient descent, this won't perform any better than the python implementation
// and Eigen is too messy for my taste, I cba with BLAS and it's fortran dependency and oneMKL requires about 6 GiBs of space on disk so NO!

// only accepts scalar types, not interested in complex types
template<typename T = float> requires std::is_arithmetic_v<T> class matrix final {
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
        constexpr matrix() noexcept : _buffer {}, _nrows {}, _ncols {} { }
};

#endif // !__MATRIX_HPP__