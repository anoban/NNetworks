#pragma once
#ifndef __MATRIX_H__
    #define __MATRIX_H__
    #include <type_traits>
    #include <vector>

template<typename T> requires std::is_arithmetic<T>::value class matrix { 
    using iterator = T*;
    using const_iterator = const T*;

private:
        std::vector<T> buffer {};
        size_t         nrows {};
        size_t         ncols {};

    public:
        matrix(void) = delete;  // don't need a default constructor

        matrix(_In_ const size_t rows, _In_ const size_t cols) noexcept;
        
        const_iterator cbegin(void) const noexcept;
        
        const_iterator cend(void) const noexcept;
        
        iterator begin(void) noexcept;
        
        iterator end(void) noexcept;
};

#endif // !__MATRIX_H__