#pragma once
#ifndef __NEUNET_HPP__
    #define __NEUNET_HPP__
    #include <random>
    #include <type_traits>

// want to make this as versatile and extensible as possible with minimal performance overhead
template<typename T, typename U> requires std::is_arithmetic_v<T> class neunet final { };

#endif // !__NEUNET_HPP__
