#pragma once
#ifndef __EXCEPTION_HPP__
    #define __EXCEPTION_HPP__

    #include <utilities.hpp>

template<typename char_t> requires utilities::is_iostream_compatible<char_t> class nnext_error final {
    public:
        constexpr inline nnext_error() noexcept : _descr {} { }

        constexpr inline explicit nnext_error(char const* const _Message) noexcept : _descr {} {
            __std_exception_data _InitData = { _Message, true };
            __std_exception_copy(&_InitData, &_descr);
        }

        constexpr inline nnext_error(char const* const _Message, int) noexcept : _descr() { _descr._What = _Message; }

        constexpr inline nnext_error(nnext_error const& _Other) noexcept : _descr() { __std_exception_copy(&_Other._descr, &_descr); }

        constexpr inline nnext_error& operator=(nnext_error const& _Other) noexcept {
            if (this == &_Other) return *this;

            __std_exception_destroy(&_descr);
            __std_exception_copy(&_Other._descr, &_descr);
            return *this;
        }

        inline ~nnext_error() noexcept { __std_exception_destroy(&_descr); }

        [[nodiscard]] const char_t* const what() const noexcept { return _descr._What ? _descr._What : "Unknown nnext_error"; }

    private:
        __std_exception_data _descr;
};

#endif // !__EXCEPTION_HPP__