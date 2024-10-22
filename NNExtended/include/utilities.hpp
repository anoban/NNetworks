#pragma once
#include <type_traits>

// use this to constrain the template arguments to overloaded std::basic_ostream<T>::operator<<()
template<typename _Ty> concept is_iostream_output_operator_compatible = std::is_same_v<_Ty, char> || std::is_same_v<_Ty, wchar_t>;
