#pragma once
#pragma comment(lib, "Ws2_32.lib")
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
// clang-format on
#include <cstdio>
#include <iostream>
#include <optional>
#include <vector>

#include <iterator.hpp>
#include <utilities.hpp>

// NOLINTBEGIN(cppcoreguidelines-pro-type-vararg,cppcoreguidelines-pro-bounds-pointer-arithmetic)

namespace internal { // routines inside this namespace aren't meant to be used outside this header

    static std::optional<unsigned char*> open(_In_ const wchar_t* const filename, _Inout_ unsigned long* const size) noexcept {
        *size = 0;
        unsigned char* buffer {};
        LARGE_INTEGER  liFsize {
             { 0LLU, 0LLU }
        };
        const HANDLE64 hFile = ::CreateFileW(filename, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, nullptr);

        if (hFile == INVALID_HANDLE_VALUE) {
            ::fwprintf_s(
                stderr,
                L"Cannot open file %s, Error %4lu occurred inside %s, in call to CreateFileW\n",
                filename,
                ::GetLastError(),
                __FUNCTIONW__
            );
            goto INVALID_HANDLE_ERR;
        }

        if (!::GetFileSizeEx(hFile, &liFsize)) {
            ::fwprintf_s(stderr, L"Error %4lu occurred inside %s, in call to GetFileSizeEx\n", ::GetLastError(), __FUNCTIONW__);
            goto GET_FILESIZE_ERR;
        }

        buffer = new (std::nothrow) unsigned char[liFsize.QuadPart];
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

    static bool serialize(
        _In_ const wchar_t* const filename, _In_ const unsigned char* const buffer, _In_ const unsigned long size
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

} // namespace internal

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

    // since most idx datasets use 8 bit unsigned integers as the value type, we'll opt to use that type

    class idx1 final {
            // [offset] [type]          [value]          [description]
            // 0000     32 bit integer  0x00000801(2049) magic number (MSB first)
            // 0004     32 bit integer  60000            number of items
            // 0008     unsigned byte   ??               label
            // 0009     unsigned byte   ??               label
            // ........
            // xxxx     unsigned byte   ??               label

        public:
            using value_type      = unsigned char;
            using pointer         = value_type*;
            using const_pointer   = const value_type*;
            using reference       = value_type&;
            using const_reference = const value_type&;
            using iterator        = random_access_iterator<value_type>;
            using const_iterator  = random_access_iterator<const value_type>;

            // clang-format off
    #ifndef __TEST__
        private:
    #endif
            // clang-format on
            unsigned       _idxmagic;   // first 4 bytes
            unsigned       _nlabels;    // next 4 bytes
            unsigned char* _raw_buffer; // the whole file buffer
            pointer        _labels; // an array of elements of a scalar type (a type casted alias to a position, 8 strides into the buffer)

            void __stdcall __shallow_copy_members(_In_ const idx1& other) noexcept {
                _idxmagic   = other._idxmagic;
                _nlabels    = other._nlabels;
                _raw_buffer = other._raw_buffer;
                _labels     = other._labels;
            }

            void __stdcall __cleanup() noexcept { // self cleanup
                _idxmagic = _nlabels = 0;
                delete[] _raw_buffer;
                _raw_buffer = _labels = nullptr;
            }

            static void __stdcall __cleanup(_Inout_ idx1& other) noexcept { // cleanp up moved from idx1 objects
                other._idxmagic = other._nlabels = 0;
                other._raw_buffer = other._labels = nullptr;
            }

        public: // NOLINT(readability-redundant-access-specifiers)
            idx1() noexcept : _idxmagic(), _nlabels(), _raw_buffer(), _labels() { }

            explicit idx1(_In_ const wchar_t* const path) noexcept : _idxmagic(), _nlabels(), _raw_buffer(), _labels() {
                unsigned long sz {};
                const auto    option { internal::open(path, &sz) };
                assert(sz >= 100); // the 100 here is an arbitrary choice

                if (!option.has_value()) {
                    ::fputws(L"" __FUNCSIG__ " failed!, object is left default initialized!\n", stderr);
                    return;
                }

                unsigned char* buffer { option.value() };
                assert(buffer);
                assert(buffer[3] == 0x01); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic) must be 0x01 for idx1 objects

                _idxmagic   = ::ntohl(*reinterpret_cast<const unsigned*>(buffer));
                _nlabels    = ::ntohl(*reinterpret_cast<const unsigned*>(buffer + 4));
                _raw_buffer = buffer;
                _labels     = reinterpret_cast<pointer>(buffer + 8);
            }

            explicit idx1(_In_ unsigned char* const buffer, _In_ const size_t& size) noexcept {
                assert(buffer);
                assert(size >= 100);
                // again, the 100 here is an arbitrary choice, when you pass a dummy buffer for testing, make sure it's longer than 100 bytes
                assert(buffer[3] == 0x01); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

                _idxmagic   = ::ntohl(*reinterpret_cast<const unsigned*>(buffer));
                _nlabels    = ::ntohl(*reinterpret_cast<const unsigned*>(buffer + 4));
                _raw_buffer = buffer;
                _labels     = reinterpret_cast<pointer>(buffer + 8);
            }

            idx1(_In_ const idx1& other) noexcept : _idxmagic(other._idxmagic), _nlabels(other._nlabels), _raw_buffer(), _labels() {
                unsigned char* temp_buff = new (std::nothrow) unsigned char[_nlabels * sizeof(value_type) + 8];
                // cannot just use value_type[_nlabels] because we need to accomodate the metadata which is always unsigned char
                // 8 for the first 8 metadata bytes and the rest for the buffer downstream
                if (!temp_buff) {
                    ::fputws(L"" __FUNCSIG__ " failed!, object is in unusable state!\n", stderr);
                    return;
                }

                std::copy(other._raw_buffer, other._raw_buffer + _nlabels * sizeof(value_type) + 8, temp_buff);
                _raw_buffer = temp_buff;
                _labels     = reinterpret_cast<pointer>(_raw_buffer + 8);
            }

            idx1(_In_ idx1&& other) noexcept :
                _idxmagic(other._idxmagic), _nlabels(other._nlabels), _raw_buffer(other._raw_buffer), _labels(other._labels) {
                __cleanup(other);
            }

            idx1& operator=(const idx1& other) noexcept {
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
                    _raw_buffer = new (std::nothrow) unsigned char[8 + other._nlabels * sizeof(value_type)];
                    if (!_raw_buffer) {
                        ::fputws(L"" __FUNCSIG__ " failed!, object is left in unusable state!\n", stderr);
                        return *this;
                    }
                    std::copy(other._raw_buffer, other._raw_buffer + 8 + other._nlabels * sizeof(value_type), _raw_buffer);
                }

                return *this;
            }

            idx1& operator=(idx1&& other) noexcept {
                if (this == &other) return *this;

                delete[] _raw_buffer; // discard the old buffer and take ownership of the stolen buffer
                __shallow_copy_members(other);
                __cleanup(other);

                return *this;
            }

            ~idx1() noexcept {
                delete[] _raw_buffer;
                _raw_buffer = _labels = nullptr;
                _idxmagic = _nlabels = 0;
            }

            iterator begin() noexcept { // NOLINT(readability-make-member-function-const)
                return { _labels, _nlabels };
            }

            const_iterator begin() const noexcept { return { _labels, _nlabels }; }

            const_iterator cbegin() const noexcept { return { _labels, _nlabels }; }

            iterator end() noexcept { // NOLINT(readability-make-member-function-const)
                return { _labels, _nlabels, _nlabels };
            }

            const_iterator end() const noexcept { return { _labels, _nlabels, _nlabels }; }

            const_iterator cend() const noexcept { return { _labels, _nlabels, _nlabels }; }

            template<typename _Ty>
            // this will trim off the first 8 bytes of idx1 object used to store the metadata and return the label buffer
            typename std::enable_if<std::is_arithmetic_v<_Ty>, std::vector<_Ty>>::type labels_astype() const noexcept {
                std::vector<_Ty> temp(_nlabels);
                std::copy(_labels, _labels + _nlabels, temp.data());
                return temp;
            }

            template<typename char_t>
            friend typename std::enable_if<::is_iostream_output_operator_compatible<char_t>, std::basic_ostream<char_t>&>::type operator<<(
                std::basic_ostream<char_t>& ostr, const idx1& object
            ) {
                if constexpr (std::is_same_v<char, char_t>)
                    ostr << "idxio::idx1 [ " << object._idxmagic << ' ' << object._nlabels << " ]\n";
                else if constexpr (std::is_same_v<wchar_t, char_t>)
                    ostr << L"idxio::idx1 [ " << object._idxmagic << L' ' << object._nlabels << L" ]\n";
                return ostr;
            }

            unsigned count() const noexcept { return _nlabels; }

            unsigned magic() const noexcept { return _idxmagic; }
    };

    class idx3 final {
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
            using value_type      = unsigned char;
            using pointer         = value_type*;
            using const_pointer   = const value_type*;
            using reference       = value_type&;
            using const_reference = const value_type&;
            using iterator        = random_access_iterator<value_type>;
            using const_iterator  = random_access_iterator<const value_type>;

            // clang-format off
    #ifndef __TEST__
        private:
    #endif
            // clang-format on
            unsigned       _idxmagic;   // first 4 bytes
            unsigned       _nimages;    // next 4 bytes
            unsigned       _nrows;      // next 4 bytes
            unsigned       _ncols;      // next 4 bytes
            unsigned char* _raw_buffer; // the whole file
            pointer        _pixels; // an array of elements of a scalar type (a type casted alias to a position, 16 strides into the buffer)

            void __stdcall __shallow_copy_members(_In_ const idx3& other) noexcept {
                _idxmagic   = other._idxmagic;
                _nimages    = other._nimages;
                _nrows      = other._nrows;
                _ncols      = other._ncols;
                _raw_buffer = other._raw_buffer;
                _pixels     = other._pixels;
            }

            void __stdcall __cleanup() noexcept { // self cleanup
                delete[] _raw_buffer;
                _idxmagic = _nimages = _nrows = _ncols = 0;
                _raw_buffer = _pixels = nullptr;
            }

            void __stdcall __cleanup(_Inout_ idx3& other) noexcept { // cleanp up moved from idx1 objects
                other._idxmagic = other._nimages = _nrows = _ncols = 0;
                other._raw_buffer = other._pixels = nullptr;
            }

        public:
            idx3() noexcept : _idxmagic(), _nimages(), _nrows(), _ncols(), _raw_buffer(), _pixels() { }

            explicit idx3(_In_ const wchar_t* const path) noexcept : _idxmagic(), _nimages(), _nrows(), _ncols(), _raw_buffer(), _pixels() {
                unsigned long sz {};
                const auto    option { internal::open(path, &sz) };
                assert(sz >= 100); // the 100 here is an arbitrary choice

                if (!option.has_value()) {
                    ::fputws(L"" __FUNCSIG__ " failed!, object is left default initialized!\n", stderr);
                    return;
                }

                unsigned char* buffer { option.value() };
                assert(buffer);
                assert(buffer[3] == 0x03); // // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic) must be 0x03 for idx3 objects

                _idxmagic   = ::ntohl(*reinterpret_cast<const unsigned*>(buffer));
                _nimages    = ::ntohl(*reinterpret_cast<const unsigned*>(buffer + 4));
                _nrows      = ::ntohl(*reinterpret_cast<const unsigned*>(buffer + 8));
                _ncols      = ::ntohl(*reinterpret_cast<const unsigned*>(buffer + 12));
                _raw_buffer = buffer;
                _pixels     = reinterpret_cast<pointer>(buffer + 16);
            }

            explicit idx3(_In_ unsigned char* const buffer, _In_ const size_t& size) noexcept {
                assert(buffer);
                assert(size >= 100);
                assert(buffer[3] == 0x03); //// NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

                _idxmagic   = ::ntohl(*reinterpret_cast<const unsigned*>(buffer));
                _nimages    = ::ntohl(*reinterpret_cast<const unsigned*>(buffer + 4));
                _nrows      = ::ntohl(*reinterpret_cast<const unsigned*>(buffer + 8));
                _ncols      = ::ntohl(*reinterpret_cast<const unsigned*>(buffer + 12));
                _raw_buffer = buffer;
                _pixels     = reinterpret_cast<pointer>(buffer + 16);
            }

            idx3(_In_ const idx3& other) noexcept :
                _idxmagic(other._idxmagic), _nimages(other._nimages), _nrows(other._nrows), _ncols(other._ncols), _raw_buffer(), _pixels() {
                // handle copying
            }

            idx3(_In_ idx3&& other) noexcept :
                _idxmagic(other._idxmagic),
                _nimages(other._nimages),
                _nrows(other._nrows),
                _ncols(other._ncols),
                _raw_buffer(other._raw_buffer),
                _pixels(other._pixels) {
                __cleanup(other);
            }

            idx3& operator=(const idx3& other) noexcept {
                if (this == &other) return *this;

                _idxmagic = other._idxmagic;
                _nimages  = other._nimages;
                _nrows    = other._nrows;
                _ncols    = other._ncols;

                // if the existing buffer can hold the other object, reuse the old buffer
                if (_nimages * _nrows * _ncols > other._nimages * other._nrows * other._ncols) { // just copy the data
                    ::memset(_raw_buffer, 0, 8LL + _nimages * _nrows * _ncols * sizeof(value_type));
                    std::copy(other._raw_buffer, other._raw_buffer + 8 + other._nimages * sizeof(value_type), _raw_buffer);
                } else if (_nimages * _nrows * _ncols == other._nimages * other._nrows * other._ncols) { // don't bother the memset
                    std::copy(
                        other._raw_buffer,
                        other._raw_buffer + 8 + other._nimages * other._nrows * other._ncols * sizeof(value_type),
                        _raw_buffer
                    );
                } else {
                    delete[] _raw_buffer;
                    _raw_buffer = new (std::nothrow) unsigned char[8 + other._nimages * other._nrows * other._ncols * sizeof(value_type)];
                    if (!_raw_buffer) {
                        ::fputws(L"" __FUNCSIG__ " failed!, object is left in unusable state!\n", stderr);
                        return *this;
                    }
                    std::copy(
                        other._raw_buffer,
                        other._raw_buffer + 8 + other._nimages * other._nrows * other._ncols * sizeof(value_type),
                        _raw_buffer
                    );
                }

                return *this;
            }

            idx3& operator=(idx3&& other) noexcept {
                if (this == &other) return *this;

                delete[] _raw_buffer; // lose the old buffer
                __shallow_copy_members(other);
                __cleanup(other);

                return *this;
            }

            ~idx3() noexcept { __cleanup(); }

            iterator begin() noexcept { return { _pixels, _nimages * _nrows * _ncols }; }

            const_iterator begin() const noexcept { return { _pixels, _nimages * _nrows * _ncols }; }

            const_iterator cbegin() const noexcept { return { _pixels, _nimages * _nrows * _ncols }; }

            iterator end() noexcept { return { _pixels, _nimages * _nrows * _ncols, _nimages * _nrows * _ncols }; }

            const_iterator end() const noexcept { return { _pixels, _nimages * _nrows * _ncols, _nimages * _nrows * _ncols }; }

            const_iterator cend() const noexcept { return { _pixels, _nimages * _nrows * _ncols, _nimages * _nrows * _ncols }; }

            template<typename _Ty> //requires std::is_arithmetic_v<_Ty>
            std::vector<_Ty> pixels_astype() const noexcept {
                std::vector<_Ty> temp(_ncols * _nrows * _nimages);
                std::copy(_pixels, _pixels + _ncols * _nrows * _nimages, temp.data());
                return temp;
            }

            template<typename char_t>
            friend typename std::enable_if<::is_iostream_output_operator_compatible<char_t>, std::basic_ostream<char_t>&>::type operator<<(
                std::basic_ostream<char_t>& ostr, const idx3& object
            ) {
                if constexpr (std::is_same_v<char, char_t>)
                    ostr << "idxio::idx3 [ " << object._idxmagic << ' ' << object._nimages << " (" << object._nrows << ", " << object._ncols
                         << ") ]\n";
                else if constexpr (std::is_same_v<wchar_t, char_t>)
                    ostr << L"idxio::idx3 [ " << object._idxmagic << L' ' << object._nimages << L" (" << object._nrows << L", "
                         << object._ncols << L") ]\n";
                return ostr;
            }

            unsigned count() const noexcept { return _nimages; }

            unsigned magic() const noexcept { return _idxmagic; }

            std::pair<unsigned, unsigned> dim() const noexcept { return { _nrows, _ncols }; }
    };

} // namespace idxio

#ifndef __TEST__
    #define internal FALSE
// we do not want the functions inside namespace internal to be accessible in the source files
// but this will wreak havoc with Google test, so do not do this when testing because gtest also uses "internal" an identifier
#endif // !__TEST__

// NOLINTEND(cppcoreguidelines-pro-type-vararg,cppcoreguidelines-pro-bounds-pointer-arithmetic)
