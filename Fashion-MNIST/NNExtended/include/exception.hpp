#pragma once
#ifndef __EXCEPTION_HPP__
    #define __EXCEPTION_HPP__

    #include <utilities.hpp>

template<typename char_t> requires utilities::is_iostream_compatible<char_t> class nnext_error final {
    public:
        constexpr inline nnext_error() noexcept : _descr {} { }

        constexpr inline explicit nnext_error(const char_t* const _detail) noexcept : _descr {} { }

        constexpr inline explicit nnext_error(const std::basic_string<char_t>& _detail) noexcept : _descr {} { }

        constexpr inline nnext_error(char const* const _detail, int) noexcept : _descr {} { }

        constexpr inline nnext_error(nnext_error const& _Other) noexcept : _descr {} { }

        constexpr inline nnext_error& operator=(nnext_error const& _Other) noexcept {
            if (this == &_Other) return *this;

            return *this;
        }

        inline ~nnext_error() noexcept { }

        [[nodiscard]] const char_t* const what() const noexcept { }

    private:
        char_t* _descr; // exception resource for storing exception descriptions
        bool    is_heap_stored;
};

#endif // !__EXCEPTION_HPP__