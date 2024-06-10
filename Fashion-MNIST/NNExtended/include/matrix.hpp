#pragma once
#ifndef __MATRIX_HPP__
    #define __MATRIX_HPP__
    #include <algorithm>
    #include <cstdint>
    #include <ranges>
    #include <type_traits>

    #include <iterator.hpp>

template<typename T, typename = typename std::enable_if<std::is_arithmetic_v<T>, T>::type> class matrix {
    public:
        using value_type      = typename std::remove_cv_t<T>;
        using pointer         = value_type*;
        using const_pointer   = const value_type*;
        using reference       = value_type&;
        using const_reference = const value_type&;
        using difference_type = ptrdiff_t;
        using size_type       = unsigned long long;
        using iterator        = random_access_iterator<value_type>;
        using const_iterator  = random_access_iterator<const value_type>;

    private:
        size_type   _nrows; // number of rows in the matrix
        size_type   _ncols; // number of columns in the matrix
        value_type* _rsrc;  // resource buffer

    public:
        // all ctors will require an explicit template type specification
        constexpr matrix() noexcept : _nrows {}, _ncols {}, _rsrc {} { }

        constexpr explicit matrix(const size_type& _dim) noexcept : _nrows { _dim }, _ncols { _dim }, _rsrc {} { }

        constexpr matrix(const size_type& _rows, const size_type& _cols) noexcept : _nrows {}, _ncols {}, _rsrc {} { }
};

#endif // !__MATRIX_HPP__
