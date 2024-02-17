#pragma once
#ifndef __IDXIO_H_
    #define __IDXIO_H_
    #define _AMD64_
    #define WIN32_LEAN_AND_MEAN
    #define WIN32_EXTRA_MEAN
// clang-format off
    #include <windef.h>        // needs to be the first include
    #include <errhandlingapi.h>
    #include <fileapi.h>
    #include <memory.h>
    #include <stdint.h>
    #include <stdio.h>
// clang-format on
/*
    Idx magic number decoding:
    The magic number is an integer (MSB first). The first 2 bytes are always 0.
    The third byte codes the type of the data:
            0x08: unsigned byte
            0x09: signed byte
            0x0B: short (2 bytes)
            0x0C: int (4 bytes)
            0x0D: float (4 bytes)
            0x0E: double (8 bytes)
    The 4-th byte codes the number of dimensions of the vector/matrix: 1 for vectors, 2 for matrices....
    The sizes in each dimension are 4-byte integers (MSB first BE, like in most non-Intel processors). 
*/

typedef enum { UBYTE = 0x08, BYTE, SHORT = 0x0B, INT, FLOAT, DOUBLE } IDX_DTYPE;
typedef enum { VECTOR = 1U, METRIX, TENSOR } IDX_CONTENT_TYPE;

typedef struct idx1 {
        /* LABEL FILE (train-labels-idx1-ubyte):
           [offset] [type]          [value]          [description]
           0000     32 bit integer  0x00000801(2049) magic number (MSB first)
           0004     32 bit integer  60000            number of items
           0008     unsigned byte   ??               label
           0009     unsigned byte   ??               label
           ........
           xxxx     unsigned byte   ??               label         
           The labels values are 0 to 9.
        */
        uint32_t magic;
        uint32_t nlabels;
        uint8_t* data;
} idx1_t;

typedef struct idx3 {
        /* IMAGE FILE (train-images-idx3-ubyte):
           [offset] [type]          [value]          [description]
           0000     32 bit integer  0x00000803(2051) magic number
           0004     32 bit integer  60000            number of images
           0008     32 bit integer  28               number of rows
           0012     32 bit integer  28               number of columns
           0016     unsigned byte   ??               pixel
           0017     unsigned byte   ??               pixel
           ........
           xxxx     unsigned byte   ??               pixel
           Pixels are organized row-wise. Pixel values are 0 to 255. 0 means background (white), 255 means foreground (black).
        */
        uint32_t magic;
        uint32_t nitems;
        uint32_t nrows_img;
        uint32_t ncols_img;
        uint8_t* data;
} idx3_t;

// declarations
idx1_t openidx1(_In_ const wchar_t* const restrict file_name);
idx3_t openidx3(_In_ const wchar_t* const restrict file_name);

#endif // !__IDXIO_H_
