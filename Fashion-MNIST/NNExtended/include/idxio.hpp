#pragma once
#ifndef __IDXIO_HPP__
    #define __IDXIO_HPP__

// clang-format off
    #define _AMD64_ // architecture
    #define WIN32_LEAN_AND_MEAN
    #define WIN32_EXTRA_MEAN
    #define NOMINMAX
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
    #include <misc.hpp>

inline namespace helpers { // routines inside namespace helpers aren't meant to be used outside this header

    [[nodiscard]] static inline std::optional<uint8_t*> __cdecl open(
        _In_ const wchar_t* const filename, _Inout_ unsigned long* const size
    ) noexcept {
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

    [[nodiscard]] static inline bool __cdecl serialize(
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
    constexpr bool is_idx_compatible_v = std::_Is_any_of_v<std::remove_cv_t<T>, unsigned char, signed char, short, int, float, double>;

    template<typename T> struct is_idx_compatible final : std::integral_constant<bool, is_idx_compatible_v<T>> { };

    template<
        typename scalar_t = uint8_t, // the element type of the idx1 buffer
        typename          = typename std::enable_if<is_idx_compatible<scalar_t>::value, bool>::type>
    class idx1 final {
            //////////////////////////
            //  IDX1 BINARY LAYOUT  //
            //////////////////////////

            // [offset] [type]          [value]          [description]
            // 0000     32 bit integer  0x00000801(2049) magic number (MSB first)
            // 0004     32 bit integer  60000            number of items
            // 0008     unsigned byte   ??               label
            // 0009     unsigned byte   ??               label
            // ........
            // xxxx     unsigned byte   ??               label

        public:
            using value_type      = scalar_t;
            using pointer         = scalar_t*;
            using const_pointer   = const scalar_t*;
            using reference       = scalar_t&;
            using const_reference = const scalar_t&;
            using iterator        = random_access_iterator<scalar_t>;
            using const_iterator  = random_access_iterator<const scalar_t>;

            // clang-format off
    #if !defined(_DEBUG) && !defined(__TEST__)
        private:
    #endif
            // clang-format on
            uint32_t _idxmagic;   // first 4 bytes
            uint32_t _nlabels;    // next 4 bytes
            uint8_t* _raw_buffer; // the whole file
            pointer  _labels;     // an array of elements of a scalar type (a type casted alias to a position, 8 strides into the buffer)

        public:
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

                uint8_t* buffer { option.value() };
                assert(buffer);
                assert(buffer[3] == 0x01); // must be 0x01 for idx1 objects

                _idxmagic   = ::ntohl(*reinterpret_cast<const uint32_t*>(buffer));
                _nlabels    = ::ntohl(*reinterpret_cast<const uint32_t*>(buffer + 4));
                _raw_buffer = buffer;
                _labels     = reinterpret_cast<pointer>(buffer + 8);
            }

            constexpr inline explicit __cdecl idx1(_In_ uint8_t* const buffer, _In_ const size_t& size) noexcept {
                assert(buffer);
                assert(size >= 100);
                // again, the 100 here is an arbitrary choice, when you pass a dummy buffer for testing, make sure it's longer than 100 bytes
                assert(buffer[3] == 0x01);

                _idxmagic   = ::ntohl(*reinterpret_cast<const uint32_t*>(buffer));
                _nlabels    = ::ntohl(*reinterpret_cast<const uint32_t*>(buffer + 4));
                _raw_buffer = buffer;
                _labels     = reinterpret_cast<pointer>(buffer + 8);
            }

            constexpr inline __cdecl idx1(_In_ const idx1& other) noexcept :
                _idxmagic(other._idxmagic), _nlabels(other._nlabels), _raw_buffer(), _labels() {
                uint8_t* temp_buff = new (std::nothrow) uint8_t[_nlabels * sizeof(value_type) + 8];
                // cannot just use value_type[_nlabels] because we need to accomodate the metadata which is always unsigned char
                // 8 for the first 8 metadata bytes and the rest for the buffer downstream
                if (!temp_buff) {
                    ::fputws(L"idx1(_In_ const idx1& other) copy constructor failed!, object is in unusable state!\n", stderr);
                    return;
                }

                std::copy(other._raw_buffer, other._raw_buffer + _nlabels * sizeof(value_type) + 8, temp_buff);
                _raw_buffer = temp_buff;
                _labels     = reinterpret_cast<pointer>(_raw_buffer + 8);
            }

            constexpr inline __cdecl idx1(_In_ idx1&& other) noexcept :
                _idxmagic(other._idxmagic), _nlabels(other._nlabels), _raw_buffer(other._raw_buffer), _labels(other._labels) {
                other._idxmagic = other._nlabels = 0;
                other._raw_buffer = other._labels = nullptr;
            }

            constexpr inline idx1& __cdecl operator=(const idx1& other) noexcept {
                if (this == &other) return *this;

                _idxmagic = other._idxmagic;
                _nlabels  = other._nlabels;

                // if the existing buffer can hold the other object, reuse the buffer
                if (_nlabels > other._nlabels) { // just copy the data
                    ::memset(_raw_buffer, 0, 8 + _nlabels * sizeof(value_type));
                    std::copy(other._raw_buffer, other._raw_buffer + 8 + other._nlabels * sizeof(value_type), _raw_buffer);
                } else if (_nlabels == other._nlabels) { // don't bother the memset
                    std::copy(other._raw_buffer, other._raw_buffer + 8 + other._nlabels * sizeof(value_type), _raw_buffer);
                } else {
                    delete[] _raw_buffer;
                    _raw_buffer = new (std::nothrow) uint8_t[8 + other._nlabels * sizeof(value_type)];
                    if (!_raw_buffer) {
                        ::fputws(L"operator=(const idx1& other) copy assignment operator failed!, object is in unusable state!\n", stderr);
                        return *this;
                    }
                    std::copy(other._raw_buffer, other._raw_buffer + 8 + other._nlabels * sizeof(value_type), _raw_buffer);
                }

                return *this;
            }

            constexpr inline idx1& __cdecl operator=(idx1&& other) noexcept {
                if (this == &other) return *this;

                delete[] _raw_buffer; // discard the old buffer and take ownership of the stolen buffer
                _idxmagic       = other._idxmagic;
                _nlabels        = other._nlabels;
                _raw_buffer     = other._raw_buffer;
                _labels         = other._labels;

                other._idxmagic = other._nlabels = 0;
                other._raw_buffer = other._labels = nullptr;

                return *this;
            }

            constexpr inline __cdecl ~idx1() noexcept {
                delete[] _raw_buffer;
                _raw_buffer = _labels = nullptr;
                _idxmagic = _nlabels = 0;
            }

            [[nodiscard]] constexpr iterator __cdecl begin() noexcept { return { _labels, _nlabels }; }

            [[nodiscard]] constexpr const_iterator __cdecl begin() const noexcept { return { _labels, _nlabels }; }

            [[nodiscard]] constexpr const_iterator __cdecl cbegin() const noexcept { return { _labels, _nlabels }; }

            [[nodiscard]] constexpr iterator __cdecl end() noexcept { return { _labels, _nlabels, _nlabels }; }

            [[nodiscard]] constexpr const_iterator __cdecl end() const noexcept { return { _labels, _nlabels, _nlabels }; }

            [[nodiscard]] constexpr const_iterator __cdecl cend() const noexcept { return { _labels, _nlabels, _nlabels }; }

            template<typename other_t>
            requires std::is_arithmetic_v<other_t> // this will discard the first 8 bytes of idx1 object used to store the metadata
            [[nodiscard("very expensive")]] std::vector<other_t> __cdecl labels_astype() const noexcept {
                std::vector<other_t> temp(_nlabels);
                std::copy(_labels, _labels + _nlabels, temp.data());
                return temp;
            }

            template<typename char_t> requires ::is_iostream_output_operator_compatible<char_t>
            friend std::basic_ostream<char_t>& __cdecl operator<<(std::basic_ostream<char_t>& ostr, const idx1& object) {
                if constexpr (std::is_same_v<char, char_t>)
                    ostr << "idxio::idx1 [ " << object._idxmagic << ' ' << object._nlabels << " ]\n";
                else if constexpr (std::is_same_v<wchar_t, char_t>)
                    ostr << L"idxio::idx1 [ " << object._idxmagic << L' ' << object._nlabels << L" ]\n";
                return ostr;
            }

            constexpr uint32_t __cdecl count() const noexcept { return _nlabels; }

            constexpr uint32_t __cdecl magic() const noexcept { return _idxmagic; }
    };

    template<typename scalar_t = uint8_t, typename = std::enable_if<std::is_arithmetic_v<scalar_t>, bool>::type> class idx3 final {
            //////////////////////////
            //  IDX3 BINARY LAYOUT  //
            //////////////////////////

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
            using value_type      = scalar_t;
            using pointer         = scalar_t*;
            using const_pointer   = const scalar_t*;
            using reference       = scalar_t&;
            using const_reference = const scalar_t&;
            using iterator        = random_access_iterator<scalar_t>;
            using const_iterator  = random_access_iterator<const scalar_t>;

            // clang-format off
    #if !defined(_DEBUG) && !defined(__TEST__)
        private:
    #endif
            // clang-format on
            uint32_t _idxmagic;   // first 4 bytes
            uint32_t _nimages;    // next 4 bytes
            uint32_t _nrows;      // next 4 bytes
            uint32_t _ncols;      // next 4 bytes
            uint8_t* _raw_buffer; // the whole file
            pointer  _pixels;     // an array of elements of a scalar type (a type casted alias to a position, 16 strides into the buffer)

        public:
            constexpr inline __cdecl idx3() noexcept : _idxmagic(), _nimages(), _nrows(), _ncols(), _raw_buffer(), _pixels() { }

            constexpr inline explicit __cdecl idx3(_In_ const wchar_t* const path) noexcept :
                _idxmagic(), _nimages(), _nrows(), _ncols(), _raw_buffer(), _pixels() {
                unsigned long                 sz {};
                const std::optional<uint8_t*> option { ::open(path, &sz) };
                assert(sz >= 100); // the 100 here is an arbitrary choice

                if (!option.has_value()) {
                    ::fputws(L"idx3(_In_ const wchar_t* const path) constructor failed!, object is left default initialized!\n", stderr);
                    return;
                }

                uint8_t* buffer { option.value() };
                assert(buffer);
                assert(buffer[3] == 0x03); // must be 0x03 for idx3 objects

                _idxmagic   = ::ntohl(*reinterpret_cast<const uint32_t*>(buffer));
                _nimages    = ::ntohl(*reinterpret_cast<const uint32_t*>(buffer + 4));
                _nrows      = ::ntohl(*reinterpret_cast<const uint32_t*>(buffer + 8));
                _ncols      = ::ntohl(*reinterpret_cast<const uint32_t*>(buffer + 12));
                _raw_buffer = buffer;
                _pixels     = reinterpret_cast<pointer>(buffer + 16);
            }

            constexpr inline explicit __cdecl idx3(_In_ uint8_t* const buffer, _In_ const size_t& size) noexcept {
                assert(buffer);
                assert(size >= 100);
                assert(buffer[3] == 0x03); // must be 0x03 for idx3 objects

                _idxmagic   = ::ntohl(*reinterpret_cast<const uint32_t*>(buffer));
                _nimages    = ::ntohl(*reinterpret_cast<const uint32_t*>(buffer + 4));
                _nrows      = ::ntohl(*reinterpret_cast<const uint32_t*>(buffer + 8));
                _ncols      = ::ntohl(*reinterpret_cast<const uint32_t*>(buffer + 12));
                _raw_buffer = buffer;
                _pixels     = reinterpret_cast<pointer>(buffer + 16);
            }

            constexpr inline __cdecl idx3(_In_ const idx3& other) noexcept :
                _idxmagic(other._idxmagic), _nimages(other._nimages), _nrows(other._nrows), _ncols(other._ncols), _raw_buffer(), _pixels() {
                // handle copying
            }

            constexpr inline __cdecl idx3(_In_ idx3&& other) noexcept :
                _idxmagic(other._idxmagic),
                _nimages(other._nimages),
                _nrows(other._nrows),
                _ncols(other._ncols),
                _raw_buffer(other._raw_buffer),
                _pixels(other._pixels) {
                // handle the cleanup
                other._idxmagic = other._nimages = other._nrows = other._ncols = 0;
                other._raw_buffer = other._pixels = nullptr;
            }

            constexpr inline idx3& __cdecl operator=(const idx3& other) noexcept {
                if (this == &other) return *this;

                _idxmagic = other._idxmagic;
                _nimages  = other._nimages;
                _nrows    = other._nrows;
                _ncols    = other._ncols;

                // if the existing buffer can hold the other object, reuse the old buffer
                if ((_nimages * _nrows * _ncols) > (other._nimages * other._nrows * other._ncols)) { // just copy the data
                    ::memset(_raw_buffer, 0, 8LL + _nimages * _nrows * _ncols * sizeof(value_type));
                    std::copy(other._raw_buffer, other._raw_buffer + 8 + other._nlabels * sizeof(value_type), _raw_buffer);
                } else if (_nlabels == other._nlabels) { // don't bother the memset
                    std::copy(other._raw_buffer, other._raw_buffer + 8 + other._nlabels * sizeof(value_type), _raw_buffer);
                } else {
                    delete[] _raw_buffer;
                    _raw_buffer = new (std::nothrow) uint8_t[8 + other._nlabels * sizeof(value_type)];
                    if (!_raw_buffer) {
                        ::fputws(L"operator=(const idx1& other) copy assignment operator failed!, object is in unusable state!\n", stderr);
                        return *this;
                    }
                    std::copy(other._raw_buffer, other._raw_buffer + 8 + other._nlabels * sizeof(value_type), _raw_buffer);
                }

                return *this;
            }

            constexpr inline idx3& __cdecl operator=(idx3&& other) noexcept {
                if (this == &other) return *this;
                delete[] _raw_buffer;

                _idxmagic       = other._idxmagic;
                _nimages        = other._nimages;
                _nrows          = other._nrows;
                _ncols          = other._ncols;
                _raw_buffer     = other._raw_buffer;
                _pixels         = other._pixels;

                other._idxmagic = other._nimages = other._nrows = other._ncols = 0;
                other._raw_buffer = other._pixels = nullptr;

                return *this;
            }

            constexpr inline __cdecl ~idx3() noexcept {
                delete[] _raw_buffer;
                _idxmagic = _nimages = _nrows = _ncols = 0;
                _raw_buffer = _pixels = nullptr;
            }

            constexpr iterator __cdecl begin() noexcept { return { _pixels, _nimages * _nrows * _ncols }; }

            constexpr const_iterator __cdecl begin() const noexcept { return { _pixels, _nimages * _nrows * _ncols }; }

            constexpr const_iterator __cdecl cbegin() const noexcept { return { _pixels, _nimages * _nrows * _ncols }; }

            constexpr iterator __cdecl end() noexcept { return { _pixels, _nimages * _nrows * _ncols, _nimages * _nrows * _ncols }; }

            constexpr const_iterator __cdecl end() const noexcept {
                return { _pixels, _nimages * _nrows * _ncols, _nimages * _nrows * _ncols };
            }

            constexpr const_iterator __cdecl cend() const noexcept {
                return { _pixels, _nimages * _nrows * _ncols, _nimages * _nrows * _ncols };
            }

            template<typename other_t> requires std::is_arithmetic_v<other_t>
            [[nodiscard("very expensive")]] std::vector<other_t> __cdecl pixels_astype() const noexcept {
                std::vector<other_t> temp(_ncols * _nrows * _nimages);
                std::copy(_pixels, _pixels + _ncols * _nrows * _nimages, temp.data());
                return temp;
            }

            template<typename char_t> requires ::is_iostream_output_operator_compatible<char_t>
            friend std::basic_ostream<char_t>& __cdecl operator<<(std::basic_ostream<char_t>& ostr, const idx3& object) {
                if constexpr (std::is_same_v<char, char_t>)
                    ostr << "idxio::idx3 [ " << object._idxmagic << ' ' << object._nimages << " (" << object._nrows << ", " << object._ncols
                         << ") ]\n";
                else if constexpr (std::is_same_v<wchar_t, char_t>)
                    ostr << L"idxio::idx3 [ " << object._idxmagic << L' ' << object._nimages << L" (" << object._nrows << L", "
                         << object._ncols << L") ]\n";
                return ostr;
            }

            constexpr uint32_t __cdecl count() const noexcept { return _nimages; }

            constexpr uint32_t __cdecl magic() const noexcept { return _idxmagic; }

            constexpr std::pair<uint32_t, uint32_t> __cdecl dim() const noexcept { return { _nrows, _ncols }; }
    };

} // namespace idxio

#endif // __IDXIO_HPP__
