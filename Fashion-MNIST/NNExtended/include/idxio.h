#pragma once
#ifndef __IDXIO_H__
    #define __IDXIO_H__
    #include <stdbool.h>
    #include <stdint.h>

typedef struct idx1 {
        /*
    [offset] [type]          [value]          [description]
    0000     32 bit integer  0x00000801(2049) magic number (MSB first)
    0004     32 bit integer  10000            number of items
    0008     unsigned byte   ??               label
    0009     unsigned byte   ??               label
    ........
    xxxx     unsigned byte   ??               label
        */

        uint32_t idxmagic; // idx magic number (0x00000801 for idx1 objects)
        uint32_t nlabels;  // number of labels in the idx1 object
        uint8_t* buffer;   // this will point to the raw file buffer
        // for performance reasons, in lieu of parsing and extracting the contents from file buffer and freeing it afterwards
        // the original file buffer will be used inside idx1 objects.
        uint8_t* labels;    // labels = buffer + 8; (offset past the metainfo)
        bool     is_usable; // check this before using idx1 objects
} idx1_t;

typedef struct idx3 {
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

        uint32_t idxmagic;       // idx magic number (0x00000803 for Idx3 objects)
        uint32_t nimages;        // number of images stored in an idx3 object
        uint32_t nrows_perimage; // height of an image, in pixels
        uint32_t ncols_perimage; // width of an image, in pixels
        uint8_t* buffer;
        uint8_t* pixels;         // pixels = buffer + 16;
        bool     is_usable;      // check this before using a idx3 object
} idx3_t;

idx1_t OpenIdx1(_In_ const wchar_t* const filename);

idx3_t OpenIdx3(_In_ const wchar_t* const filename);

bool   FreeIdx1(idx1_t* const restrict object);

bool   FreeIdx3(idx3_t* const restrict object);

void   PrintIdx1(_In_ const idx1_t* const object);

void   PrintIdx3(_In_ const idx3_t* const object);

#endif // !__IDXIO_H__
