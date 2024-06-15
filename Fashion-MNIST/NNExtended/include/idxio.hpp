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
    #include <Winsock2.h>

    #pragma comment(lib, "Ws2_32.lib")
// clang-format on

    #include <cstdint>
    #include <cstdio>
    #include <iostream>
    #include <optional>
    #include <vector>

    #include <iterator.hpp>

// use this to constrain the template arguments to overloaded std::basic_ostream<T>::<<operator()
template<typename T> concept is_iostream_output_operator_compatible = std::is_same_v<T, char> || std::is_same_v<T, wchar_t>;

inline namespace helpers { // routines inside namespace helpers aren't meant to be used outside this header

    static inline std::optional<uint8_t*> __cdecl open(_In_ const wchar_t* const filename, _Inout_ unsigned long* const size) noexcept {
        *size = 0;
        uint8_t*       buffer {};
        LARGE_INTEGER  liFsize { .QuadPart = 0LLU };
        const HANDLE64 hFile = ::CreateFileW(filename, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, nullptr);

        if (hFile == INVALID_HANDLE_VALUE) {
            ::fwprintf_s(stderr, L"Error %4lu occurred inside %s, in call to CreateFileW\n", ::GetLastError(), __FUNCTIONW__);
            goto INVALID_HANDLE_ERR;
        }

        if (!::GetFileSizeEx(hFile, &liFsize)) {
            ::fwprintf_s(stderr, L"Error %4lu occurred inside %s, in call to GetFileSizeEx\n", ::GetLastError(), __FUNCTIONW__);
            goto GET_FILESIZE_ERR;
        }

        buffer = new (std::nothrow) uint8_t[liFsize.QuadPart];
        if (!buffer) {
            ::fwprintf_s(stderr, L"Memory allocation failed inside %s, in call to new (std::nothrow)\n", __FUNCTIONW__);
            goto GET_FILESIZE_ERR;
        }

        if (!::ReadFile(hFile, buffer, liFsize.QuadPart, size, nullptr)) {
            ::fwprintf_s(stderr, L"Error %4lu occurred inside %s, in call to ReadFile\n", ::GetLastError(), __FUNCTIONW__);
            goto GET_FILESIZE_ERR;
        }

        ::CloseHandle(hFile);
        return buffer;

GET_FILESIZE_ERR:
        delete[] buffer;
        ::CloseHandle(hFile);
INVALID_HANDLE_ERR:
        return std::nullopt;
    }

    static inline bool __cdecl serialize(
        _In_ const wchar_t* const filename, _In_ const uint8_t* const buffer, _In_ const unsigned long size
    ) noexcept {
        const HANDLE64 hFile  = ::CreateFileW(filename, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        DWORD          nbytes = 0;

        if (hFile == INVALID_HANDLE_VALUE) {
            ::fwprintf_s(stderr, L"Error %4lu occurred inside %s, in call to CreateFileW\n", ::GetLastError(), __FUNCTIONW__);
            goto PREMATURE_RETURN;
        }

        if (!::WriteFile(hFile, buffer, size, &nbytes, nullptr)) {
            ::fwprintf_s(stderr, L"Error %4lu occurred inside %s, in call to WriteFile\n", ::GetLastError(), __FUNCTIONW__);
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

    // an excerpt on idx files
    // - idx1 and idx3 files all alike use BIG ENDIAN byte order for multibyte values!
    // - idx magic number is a 32 bit BIG ENDIAN unsigned value, that can be deconstructed as follows!
    // - the first two bytes are always 0 (in MSB first representation 0x0000
    // - the third byte encodes the type of data downstream, with the following mapping,
    //      0x08 : unsigned byte
    //      0x09 : signed byte
    //      0x0B : short(2 bytes)
    //      0x0C : int(4 bytes)
    //      0x0D : float(4 bytes)
    //      0x0E : double(8 bytes)
    // - the fourth byte encodes the number of dimensions, 0x01 for vectors, 0x02 for matrices and 0x03 for tensors (presumably)
    // for the ubyte variants of idx files, the data type will be unsigned chars, hence the third byte of the magic number will be 0x08

    template<typename T>
    constexpr bool is_idx_compatible_v = std::_Is_any_of_v<std::remove_cv_t<T>, unsigned char, char, short, int, float, double>;

    template<typename T> struct is_idx_compatible final : std::integral_constant<bool, is_idx_compatible_v<T>> { };

    template<typename scalar_t, typename = typename std::enable_if<is_idx_compatible<scalar_t>::value, bool>::type> class idx1 final {
            // [offset] [type]          [value]          [description]
            // 0000     32 bit integer  0x00000801(2049) magic number (MSB first)
            // 0004     32 bit integer  60000            number of items
            // 0008     unsigned byte   ??               label
            // 0009     unsigned byte   ??               label
            // ........
            // xxxx     unsigned byte   ??               label

        public:
            using value_type      = typename std::remove_cv<scalar_t>::type;
            using pointer         = scalar_t*;
            using const_pointer   = const value_type*;
            using reference       = scalar_t&;
            using const_reference = const value_type&;
            using iterator        = random_access_iterator<value_type>;
            using const_iterator  = random_access_iterator<const value_type>;

        private:
            uint32_t       _idxmagic;   // first 4 bytes
            uint32_t       _nlabels;    // next 4 bytes
            const uint8_t* _raw_buffer; // the whole file
            const_pointer  _labels; // an array of elements of a scalar type (a type casted alias to a position, 8 strides into the buffer)

        public:
            // the template argument of ctors needs to be explicitly specified because the compiler has no way to infer it

            constexpr inline __cdecl idx1() noexcept : _idxmagic(), _nlabels(), _raw_buffer(), _labels() { }

            constexpr inline explicit __cdecl idx1(_In_ const wchar_t* const path) noexcept :
                _idxmagic(), _nlabels(), _raw_buffer(), _labels() {
                unsigned long                 sz {};
                const std::optional<uint8_t*> option { ::open(path, &sz) };
                assert(sz >= 100); // the 100 here is an arbitrary choice

                if (!option.has_value()) {
                    ::fputws(L"idx1(_In_ const wchar_t* const path) constructor failed!, object is left default initialized!\n", stderr);
                    return;
                }

                const uint8_t* buffer { option.value() };
                assert(buffer);
                assert(buffer[3] == 0x01); // must be 0x01 for idx1 objects

                _idxmagic   = ::ntohl(*reinterpret_cast<const uint32_t*>(buffer));
                _nlabels    = ::ntohl(*reinterpret_cast<const uint32_t*>(buffer + 4));
                _raw_buffer = buffer;
                _labels     = reinterpret_cast<const_pointer>(buffer + 8);
            }

            constexpr inline explicit __cdecl idx1(_In_ const uint8_t* const buffer, _In_ const size_t& size) noexcept {
                assert(buffer);
                assert(size >= 100);
                // again, the 100 here is an arbitrary choice, when you pass a dummy buffer for testing, make sure it's longer than 100 bytes

                _idxmagic   = ::ntohl(*reinterpret_cast<const uint32_t*>(buffer));
                _nlabels    = ::ntohl(*reinterpret_cast<const uint32_t*>(buffer + 4));
                _raw_buffer = buffer;
                _labels     = reinterpret_cast<const_pointer>(buffer + 8);
            }

            constexpr inline __cdecl idx1(_In_ const idx1& other) noexcept :
                _idxmagic(other._idxmagic), _nlabels(other._nlabels), _raw_buffer(), _labels() {
                // copy the buffers
            }

            constexpr inline __cdecl idx1(_In_ idx1&& other) noexcept :
                _idxmagic(other._idxmagic), _nlabels(other._nlabels), _raw_buffer(other._raw_buffer), _labels(other._labels) {
                other._idxmagic = other._nlabels = 0;
                other._raw_buffer = other._labels = nullptr;
            }

            constexpr inline idx1& __cdecl operator=(const idx1& other) noexcept { }

            constexpr inline idx1& __cdecl operator=(idx1&& other) noexcept { }

            constexpr inline __cdecl ~idx1() noexcept {
                delete[] _raw_buffer;
                _raw_buffer = _labels = nullptr;
                _idxmagic = _nlabels = 0;
            }

            constexpr iterator __cdecl begin() noexcept { }

            constexpr const_iterator __cdecl begin() const noexcept { }

            constexpr const_iterator __cdecl cbegin() const noexcept { }

            constexpr iterator __cdecl end() noexcept { }

            constexpr const_iterator __cdecl end() const noexcept { }

            constexpr const_iterator __cdecl cend() const noexcept { }

            template<typename other_t> requires std::is_arithmetic_v<other_t> std::vector<other_t> __cdecl as_type() const noexcept { }

            template<typename char_t> requires ::is_iostream_output_operator_compatible<char_t>
            friend std::basic_ostream<char_t>& __cdecl operator<<(std::basic_ostream<char_t>& ostr, const idx1& object) { }

            constexpr uint32_t __cdecl count() const noexcept { return _nlabels; }

            constexpr uint32_t __cdecl magic() const noexcept { return _idxmagic; }
    };

    template<typename scalar_t, typename = std::enable_if<std::is_arithmetic_v<scalar_t>, bool>::type> class idx3 final {
            // [offset] [type]          [value]          [description]
            // 0000     32 bit integer  0x00000803(2051) magic number
            // 0004     32 bit integer  60000            number of images
            // 0008     32 bit integer  28               number of rows
            // 0012     32 bit integer  28               number of columns
            // 0016     unsigned byte   ??               pixel
            // 0017     unsigned byte   ??               pixel
            // ........
            // xxxx     unsigned byte   ??               pixel

        public:
            using value_type      = typename std::remove_cv<scalar_t>::type;
            using pointer         = scalar_t*;
            using const_pointer   = const value_type*;
            using reference       = scalar_t&;
            using const_reference = const value_type&;
            using iterator        = random_access_iterator<value_type>;
            using const_iterator  = random_access_iterator<const value_type>;

        private:
            uint32_t       _idxmagic;   // first 4 bytes
            uint32_t       _nimages;    // next 4 bytes
            uint32_t       _nrows;      // next 4 bytes
            uint32_t       _ncols;      // next 4 bytes
            const uint8_t* _raw_buffer; // the whole file
            const_pointer  _pixels;     // an array of elements of a scalar type (a type casted alias to a position, 32 strides
            // into the buffer)

        public:
            // the template argument needs to be scecified explicitly because the compiler has no way to infer it
            // from the ctor argument types
            constexpr inline __cdecl idx3() noexcept { }

            constexpr inline explicit __cdecl idx3(_In_ const wchar_t* const path) noexcept { }

            constexpr inline explicit __cdecl idx3(_In_ const uint8_t* const buffer, _In_ const size_t& size) noexcept { }

            constexpr inline __cdecl idx3(_In_ const idx3& other) noexcept { }

            constexpr inline __cdecl idx3(_In_ idx3&& other) noexcept { }

            constexpr inline idx3& __cdecl operator=(const idx3& other) noexcept { }

            constexpr inline idx3& __cdecl operator=(idx3&& other) noexcept { }

            constexpr inline __cdecl ~idx3() noexcept { }

            constexpr iterator __cdecl begin() noexcept { }

            constexpr const_iterator __cdecl begin() const noexcept { }

            constexpr const_iterator __cdecl cbegin() const noexcept { }

            constexpr iterator __cdecl end() noexcept { }

            constexpr const_iterator __cdecl end() const noexcept { }

            constexpr const_iterator __cdecl cend() const noexcept { }

            template<typename other_t> requires std::is_arithmetic_v<other_t> std::vector<other_t> __cdecl as_type() const noexcept { }

            template<typename char_t> requires ::is_iostream_output_operator_compatible<char_t>
            friend std::basic_ostream<char_t>& __cdecl operator<<(std::basic_ostream<char_t>& ostr, const idx3& object) { }

            constexpr unsigned __cdecl count() const noexcept { return _nimages; }

            constexpr unsigned __cdecl magic() const noexcept { return _idxmagic; }
    };

    // convenience aliases for the ubyte variants of idx types which use unsigned bytes as the element type
    using idx1u8 = idx1<uint8_t>;

    using idx3u8 = idx3<uint8_t>;

} // namespace idxio

#endif // __IDXIO_HPP__
