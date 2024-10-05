#pragma once
#ifndef __MISC_HPP__
    #define __MISC_HPP__
    #include <type_traits>

// use this to constrain the template arguments to overloaded std::basic_ostream<T>::<<operator()
template<typename T> concept is_iostream_output_operator_compatible = std::is_same_v<T, char> || std::is_same_v<T, wchar_t>;

// reverses the byte order of multibyte integers
template<typename T>
[[nodiscard]] constexpr typename std::enable_if<std::is_integral<T>::value, T>::type reverse_bytes(const T& integer) noexcept {
    static_assert(sizeof(T) != 1LLU);

    T result {};
}

template<> [[nodiscard]] constexpr char reverse_bytes(const char& integer) noexcept { return integer; }

template<> [[nodiscard]] constexpr unsigned char reverse_bytes(const unsigned char& integer) noexcept { return integer; }

#endif // !__MISC_HPP__