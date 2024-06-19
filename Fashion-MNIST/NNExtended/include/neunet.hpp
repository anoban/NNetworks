#pragma once
#ifndef __NEUNET_HPP__
    #define __NEUNET_HPP__

    #include <type_traits>

template<typename T> requires std::is_arithmetic_v<T> class neunet final { };

#endif // !__NEUNET_HPP__
