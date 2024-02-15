#pragma once
#ifndef __MATRIX_H_
    #define __MATRIX_H_
    #include <stdint.h>

typedef struct matrix {
        size_t       nrows;
        size_t       ncols;
        long double* data;
} matrix_t;

#endif // !__MATRIX_H_
