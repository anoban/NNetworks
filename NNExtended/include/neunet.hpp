#pragma once
#include <random>
#include <type_traits>

// want to make this as versatile and extensible as possible with minimal performance overhead
template<typename T, typename U> requires std::is_arithmetic_v<T> class neunet final { };
