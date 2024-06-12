#pragma once
#ifndef __EXCEPTION_HPP__
    #define __EXCEPTION_HPP__

    #include <utilities.hpp>

// implementing a custom exception becase std::exception uses char* as return type for what() virtual method but I want a wchar_t* variant
// and don't want the vtable overhead!
template<typename char_t> requires utilities::is_iostream_compatible<char_t> class nnext_exception final {
    public:
        constexpr inline nnext_exception() noexcept : _descr {}, _is_heap_allocated {} { }

        constexpr inline explicit nnext_exception(const char_t* const _detail) noexcept : // when the input is a string literal
            _descr { _detail }, _is_heap_allocated { false } { }

        constexpr inline explicit nnext_exception(const std::basic_string<char_t>& _detail) noexcept : _descr {} { }

        constexpr inline explicit nnext_exception(std::basic_string<char_t>&& _detail) noexcept :
            _descr { std::swap(_detail.c_str(), _detail) } { }

        constexpr inline nnext_exception(char const* const _detail, int) noexcept : _descr {} { }

        constexpr inline nnext_exception(nnext_exception const& _Other) noexcept : _descr {} { }

        constexpr inline nnext_exception& operator=(nnext_exception const& _Other) noexcept {
            if (this == &_Other) return *this;

            return *this;
        }

        constexpr inline ~nnext_exception() noexcept {
            if (_is_heap_allocated) delete[] _descr;
            _descr = nullptr;
        }

        [[nodiscard]] const char_t* const what() const noexcept { }

    private:
        const char_t* _descr;             // exception resource for storing exception descriptions
        bool          _is_heap_allocated; // flag registering whether _descr is a heap allocated string
};

#endif // !__EXCEPTION_HPP__