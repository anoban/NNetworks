#pragma once
#ifndef __IDXIO_HPP__
    #define __IDXIO_HPP__

// clang-format off
    #define _AMD64_ // architecture
    #define WIN32_LEAN_AND_MEAN
    #define WIN32_EXTRA_MEAN
    #include <windef.h>
    #include <errhandlingapi.h>
    #include <fileapi.h>
    #include <handleapi.h>
// clang-format on

    #include <cstdint>
    #include <cstdio>
    #include <optional>
    #include <vector>

inline namespace helpers {

    static inline std::optional<std::vector<uint8_t>> __cdecl open(
        _In_ const wchar_t* const filename, _Inout_ unsigned long* const size
    ) noexcept {
        *size = 0;
        std::vector<uint8_t> buffer {};
        LARGE_INTEGER        liFsize { .QuadPart = 0LLU };
        const HANDLE64       hFile = ::CreateFileW(filename, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, nullptr);

        if (hFile == INVALID_HANDLE_VALUE) {
            ::fwprintf_s(stderr, L"Error %lu in CreateFileW\n", ::GetLastError());
            goto INVALID_HANDLE_ERR;
        }

        if (!::GetFileSizeEx(hFile, &liFsize)) {
            ::fwprintf_s(stderr, L"Error %lu in GetFileSizeEx\n", ::GetLastError());
            goto GET_FILESIZE_ERR;
        }

        buffer.resize(liFsize.QuadPart);

        if (!::ReadFile(hFile, buffer.data(), liFsize.QuadPart, size, nullptr)) {
            ::fwprintf_s(stderr, L"Error %lu in ReadFile\n", ::GetLastError());
            goto GET_FILESIZE_ERR;
        }

        ::CloseHandle(hFile);
        return buffer;

GET_FILESIZE_ERR:
        ::CloseHandle(hFile);
INVALID_HANDLE_ERR:
        return std::nullopt;
    }

    static inline bool __cdecl serialize(
        _In_ const wchar_t* const filename, _In_ const uint8_t* const buffer, _In_ const unsigned long size
    ) noexcept {
        const HANDLE64 hFile  = CreateFileW(filename, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        DWORD          nbytes = 0;

        if (hFile == INVALID_HANDLE_VALUE) {
            ::fwprintf_s(stderr, L"Error %4lu in CreateFileW\n", GetLastError());
            goto PREMATURE_RETURN;
        }

        if (!::WriteFile(hFile, buffer, size, &nbytes, nullptr)) {
            ::fwprintf_s(stderr, L"Error %4lu in WriteFile\n", GetLastError());
            goto PREMATURE_RETURN;
        }

        ::CloseHandle(hFile);
        return true;

PREMATURE_RETURN:
        ::CloseHandle(hFile);
        return false;
    }

} // namespace helpers

namespace idxio { // we will not be using exceptions here! caller will have to manually examine the returned class type for errors

    class idx1 {
        public:

        private:
            uint32_t _idxmagic;
            uint32_t _nlabels;
            uint8_t* _labels;

        public:
            constexpr inline idx1() noexcept; // default ctor

            constexpr inline explicit idx1(_In_ const wchar_t* const _path) noexcept; // construct from a file path

            constexpr inline explicit idx1(
                _In_ const uint8_t* const _buffer, _In_ const size_t& _sz
            ) noexcept; // construct from a byte buffer

            constexpr inline idx1(_In_ const idx1& _other) noexcept; // copy ctor

            constexpr inline idx1(_In_ idx1&& _other) noexcept; // move ctor

            constexpr inline idx1& operator=(const idx1& _other) noexcept; // copy =

            constexpr inline idx1& operator=(idx1&& _other) noexcept; // move =

            constexpr inline ~idx1() noexcept; // dtor

            constexpr size_t count() const noexcept; // label count
    };

    class idx3 { };
} // namespace idxio

#endif // __IDXIO_HPP__
