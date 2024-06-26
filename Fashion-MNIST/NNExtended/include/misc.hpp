#pragma once
#ifndef __MISC_HPP__
    #define __MISC_HPP__
    #include <type_traits>

// use this to constrain the template arguments to overloaded std::basic_ostream<T>::<<operator()
template<typename T> concept is_iostream_output_operator_compatible = std::is_same_v<T, char> || std::is_same_v<T, wchar_t>;

template<typename T>
[[nodiscard]] constexpr T revbyteorder(const T& integer, typename std::enable_if<std::is_integral<T>::value, bool>::type = true) noexcept {
    // reverses the byte order of multibyte integers
}

#endif // !__MISC_HPP__