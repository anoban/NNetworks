#pragma once
#ifndef __EXCEPTION_HPP__
    #define __EXCEPTION_HPP__
    #include <utilities.hpp>

// implementing a custom exception becase std::exception uses char* as return type for what() virtual method but I want a wchar_t* variant
// and don't want the vtable overhead!
template<typename char_t = wchar_t> requires utilities::is_iostream_compatible<char_t> class nnext_exception final {
    public:
        constexpr inline nnext_exception() noexcept : _descr {}, _is_heap_allocated {} { }

        // do not use heap allocated strings with this ctor, will lead to a memory leak
        constexpr inline explicit nnext_exception(const char_t* const _detail) noexcept : _descr { _detail } { }

        constexpr inline nnext_exception(const nnext_exception& _other) noexcept : _descr { _other._descr } { } // copying just the pointer

        constexpr inline nnext_exception(nnext_exception&& _other) noexcept : _descr { _other._descr } { _other._descr = nullptr; }

        constexpr inline nnext_exception& operator=(const nnext_exception& _other) noexcept {
            if (this == &_other) return *this;
            _descr = _other._descr;
            return *this;
        }

        constexpr inline nnext_exception& operator=(nnext_exception&& _other) noexcept {
            if (this == &_other) return *this;
            _descr        = _other._descr;
            _other._descr = nullptr;
            return *this;
        }

        constexpr inline ~nnext_exception() noexcept { _descr = nullptr; }

        [[nodiscard]] const char_t* const what() const noexcept { return _descr; }

    private:
        const char_t* _descr; // exception resource for storing exception descriptions
};

#endif // !__EXCEPTION_HPP__