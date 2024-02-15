#pragma once
#ifndef __IDXIO_H_
    #define __IDXIO_H_
    #include <stdint.h>

typedef struct idx1 {
        size_t   nlabels;
        uint8_t* data;
} idx1_t;

typedef struct idx3 {
        size_t   nitems;
        size_t   nrows_img;
        size_t   ncols_img;
        uint8_t* data;
} idx3_t;

#endif // !__IDXIO_H_
