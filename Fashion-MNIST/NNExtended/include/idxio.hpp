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
#include <heapapi.h>
#include <winsock.h>
#pragma comment(lib, "Ws2_32.lib") // ntohl
// clang-format on

    #include <cstdint>
    #include <cstdio>
    #include <iostream>
    #include <optional>
    #include <vector>

// to be used as a type constarint for templates that deal with iostreams, to filter types char and wchar_t
template<typename T> concept is_iostream_compatible =
    std::is_same_v<char, std::remove_cv_t<T>> || std::is_same_v<wchar_t, std::remove_cv_t<T>>;

namespace idx {

    class idx1 final {
            // [offset] [type]          [value]          [description]
            // 0000     32 bit integer  0x00000801(2049) magic number (MSB first)
            // 0004     32 bit integer  10000            number of items
            // 0008     unsigned byte   ??               label
            // 0009     unsigned byte   ??               label
            // ........
            // xxxx     unsigned byte   ??               label

        private:
            uint32_t idxmagic; // idx magic number (0x00000801 for idx1 objects)
            uint32_t nlabels;  // number of labels in the idx1 object
            uint8_t* buffer;   // this will point to the raw file buffer
            // for performance reasons, in lieu of parsing and extracting the contents from file buffer and freeing it afterwards
            // the original file buffer will be used inside idx1 objects.
            uint8_t* labels;    // labels = buffer + 8; (offset past the metainfo)
            bool     is_usable; // check this before using idx1 objects

        public:
            constexpr idx1() noexcept;

            constexpr idx1(_In_ const wchar_t* const filename) noexcept;

            constexpr idx1(_In_ const idx1& other) noexcept;

            constexpr idx1(_In_ idx1&& other) noexcept;

            constexpr idx1& operator=(_In_ const idx1& other) noexcept;

            constexpr idx1& operator=(_In_ idx1&& other) noexcept;

            constexpr ~idx1() noexcept;

            template<typename char_t> requires ::is_iostream_compatible<char_t>
            friend std::basic_ostream<char_t>& operator<<(std::basic_ostream<char_t>& ostr, const idx1& object);
    };

    class idx3 final {
            // [offset] [type]          [value]          [description]
            // 0000     32 bit integer  0x00000803(2051) magic number
            // 0004     32 bit integer  10000            number of images
            // 0008     32 bit integer  28               number of rows
            // 0012     32 bit integer  28               number of columns
            // 0016     unsigned byte   ??               pixel
            // 0017     unsigned byte   ??               pixel
            // ........
            // xxxx     unsigned byte   ??               pixel

        private:
            uint32_t idxmagic;       // idx magic number (0x00000803 for Idx3 objects)
            uint32_t nimages;        // number of images stored in an idx3 object
            uint32_t nrows_perimage; // height of an image, in pixels
            uint32_t ncols_perimage; // width of an image, in pixels
            uint8_t* buffer;
            uint8_t* pixels;         // pixels = buffer + 16;
            bool     is_usable;      // check this before using a idx3 object

        public:
            constexpr idx3() noexcept;

            constexpr idx3(_In_ const wchar_t* const filename) noexcept;

            constexpr idx3(_In_ const idx3& other) noexcept;

            constexpr idx3(_In_ idx3&& other) noexcept;

            constexpr idx3& operator=(_In_ const idx3& other) noexcept;

            constexpr idx3& operator=(_In_ idx3&& other) noexcept;

            constexpr ~idx3() noexcept;

            template<typename char_t> requires ::is_iostream_compatible<char_t>
            friend std::basic_ostream<char_t>& operator<<(std::basic_ostream<char_t>& ostr, const idx3& object);
    };

} // namespace idx

#endif // !__IDXIO_HPP__
