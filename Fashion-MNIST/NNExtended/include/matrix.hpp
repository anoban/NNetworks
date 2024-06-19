#pragma once
#ifndef __MATRIX_HPP__
    #define __MATRIX_HPP__

    #include <concepts>

template<typename T> requires std::is_arithmetic_v<T> class matrix final { };

#endif // !__MATRIX_HPP__