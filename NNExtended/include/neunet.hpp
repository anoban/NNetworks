#pragma once
#include <random>
#include <type_traits>

// want to make this as versatile and extensible as possible with minimal performance overhead
template<typename T, typename U, typename = typename std::enable_if<std::is_arithmetic_v<T>, bool>::type> class neunet final { };
