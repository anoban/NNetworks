#pragma once
#ifndef __IDXIO_H__
    #define __IDXIO_H__
    #include <concepts>
    #include <vector>

namespace idx {

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
            uint32_t             idxmagic {};
            uint32_t             nlabels {};
            std::vector<uint8_t> labels {};
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
            uint32_t             idxmagic {};
            uint32_t             nimages {};
            uint32_t             nrows_perimage {};
            uint32_t             ncols_perimage {};
            std::vector<uint8_t> buffer {};
    };

} // namespace idx

#endif // !__IDXIO_H__
