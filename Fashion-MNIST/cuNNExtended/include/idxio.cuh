#pragma once
#ifndef __IDXIO_H__
    #define __IDXIO_H__

namespace idxio {

    class idx1 {
            /*
    [offset] [type]          [value]          [description]
    0000     32 bit integer  0x00000801(2049) magic number (MSB first)
    0004     32 bit integer  10000            number of items
    0008     unsigned byte   ??               label
    0009     unsigned byte   ??               label
    ........
    xxxx     unsigned byte   ??               label
        	*/

        private:
            uint32_t idxmagic {};
            uint32_t nlabels {};
            uint8_t* buffer;
            uint8_t* labels {}; // labels = buffer + 8;
            bool     is_usable {};

        public:
            idx1(void) = delete; // we do not need a default constructor here
            inline idx1(_In_ const wchar_t* const filename) noexcept;
            ~idx1(void) noexcept;
    };

    class idx3 {
            /*
    [offset] [type]          [value]          [description]
    0000     32 bit integer  0x00000803(2051) magic number
    0004     32 bit integer  10000            number of images
    0008     32 bit integer  28               number of rows
    0012     32 bit integer  28               number of columns
    0016     unsigned byte   ??               pixel
    0017     unsigned byte   ??               pixel
    ........
    xxxx     unsigned byte   ??               pixel
            */

        private:
            uint32_t idxmagic {};
            uint32_t nimages {};
            uint32_t nrows_perimage {};
            uint32_t ncols_perimage {};
            uint8_t* buffer {};
            uint8_t* pixels {}; // pixels = buffer + 16;
            bool     is_usable {};

        public:
            idx3(void) = delete;
            inline idx3(_In_ const wchar_t* const filename) noexcept;
            ~idx3(void) noexcept;
    };

} // namespace idxio

#endif // !__IDXIO_H__
