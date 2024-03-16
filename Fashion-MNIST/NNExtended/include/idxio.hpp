#pragma once
#ifndef __IDXIO_H__
    #define __IDXIO_H__

namespace idxio {

    struct idx1 {
            /*
    [offset] [type]          [value]          [description]
    0000     32 bit integer  0x00000801(2049) magic number (MSB first)
    0004     32 bit integer  10000            number of items
    0008     unsigned byte   ??               label
    0009     unsigned byte   ??               label
    ........
    xxxx     unsigned byte   ??               label
        	*/

            using const_iterator = const uint8_t*;

        private:
            uint32_t idxmagic {}; // idx magic number (0x00000801 for idx1 objects)
            uint32_t nlabels {};  // number of labels in the idx1 object
            uint8_t* buffer {};   // this will point to the raw file buffer
            // for performance reasons, in lieu of parsing and extracting the contents from file buffer and freeing it afterwards
            // the original file buffer will be used inside idx1 objects.
            uint8_t* labels {}; // labels = buffer + 8; (offset past the metainfo)
            bool     usable {}; // check this before using idx1 objects

        public:
            idx1(void) = delete; // we do not need a default constructor here

            // pass in the file path to a Idx1 file to construct an instance of idx1 class.
            // any erros in object creation will reported on console, and if any did occur, subsequent call to is_usable will return false.
            // no exceptions will be thrown, so always check is_usable() before using a idx1 object
            idx1(_In_ const wchar_t* const filename) noexcept;

            ~idx1(void) noexcept;                                 // destructor: HeapFree()

            size_t                size(void) const noexcept;      // returns the number of labels in the object

            bool                  is_usable(void) const noexcept; // is the instance in a usable state?

            const_iterator cbegin(void) const noexcept;    // start of a constant iterator to the labels buffer

            const_iterator cend(void) const noexcept;      // end of constant iterator to the labels buffer
    };

    struct idx3 {
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

            using const_iterator = const uint8_t*;
            // the caller should be able to modify the iterator though, so const uint8_t* const won't work!

        private:
            uint32_t idxmagic {};       // idx magic number (0x00000803 for Idx3 objects)
            uint32_t nimages {};        // number of images stored in an idx3 object
            uint32_t nrows_perimage {}; // height of an image, in pixels
            uint32_t ncols_perimage {}; // width of an image, in pixels
            uint8_t* buffer {};
            uint8_t* pixels {};         // pixels = buffer + 16;
            bool     usable {};         // check this before using a idx3 object

        public:
            idx3(void) = delete;

            // similar to idx1 class, any errors in object creation will be reported on console and is_usable will be set to false
            // always check the return value of is_usable() before using idx3 objects.
            inline idx3(_In_ const wchar_t* const filename) noexcept;

            ~idx3(void) noexcept;                                 // HeapFree()

            size_t                size(void) const noexcept;      // returns the number of labels in the object

            bool                  is_usable(void) const noexcept; // is the instance in a usable state?

            inline const_iterator cbegin(void) const noexcept;    // start of a constant iterator to the labels buffer

            inline const_iterator cend(void) const noexcept;      // end of constant iterator to the labels buffer
    };

} // namespace idxio

#endif // !__IDXIO_H__
