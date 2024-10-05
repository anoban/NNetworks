#pragma once
#ifndef __MISC_HPP__
    #define __MISC_HPP__
    #include <type_traits>

// use this to constrain the template arguments to overloaded std::basic_ostream<T>::<<operator()
template<typename _Ty> concept is_iostream_output_operator_compatible = std::is_same_v<_Ty, char> || std::is_same_v<_Ty, wchar_t>;

template<typename _Ty0, typename... _TyList> struct is_in final {
        static constexpr bool value { sizeof...(_TyList) == 1 ? std::is_same<typename std::remove_cv<_Ty0>::type, _TyList>::value :
                                                                is_in<typename std::remove_cv<_Ty0>::type, _TyList...>::value };
};

template<typename _Ty> constexpr bool is_idx_compatible = ::is_in<_Ty, unsigned char, signed char, short, int, float, double>::value;

template<typename _Ty> inline constexpr bool is_idx_compatible_v = is_idx_compatible<_Ty>::value;

#endif // !__MISC_HPP__