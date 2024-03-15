#pragma once
#ifndef __IDXIO_H__
    #define __IDXIO_H__
    #include <concepts>
    #include <vector>

namespace idx {

    template<typename T> requires std::integral<T> && std::is_unsigned<T>::value class idx1 {
            // clang-format off

	/*
    [offset] [type]          [value]          [description]
    0000     32 bit integer  0x00000801(2049) magic number (MSB first)
    0004     32 bit integer  10000            number of items
    0008     unsigned byte   ??               label
    0009     unsigned byte   ??               label
    ........
    xxxx     unsigned byte   ??               label
	*/

            // clang-format on

        private:
            uint32_t       idxmagic {};
            uint32_t       nlabels {};
            std::vector<T> labels {};
    };

    template<typename T> class idx3 { };

} // namespace idx

#endif // !__IDXIO_H__
