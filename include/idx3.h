#pragma once
#ifndef __IDX3_H__
    #define __IDX3_H__
    #include <stdbool.h>
    #include <stdint.h>
    #include <stdio.h>
    #include <stdlib.h>
    #define _AMD64_
    #define WIN32_LEAN_AND_MEAN
    #define WIN32_EXTRA_MEAN
    #include <errhandlingapi.h>
    #include <fileapi.h>
    #include <handleapi.h>
    #include <windef.h>
    #include <winsock.h> // ntohl (net to host long) ntohl() converts a u_long from TCP/IP network order to host byte order
// (which is little-endian on Intel processors). It just reverses the bytes of the input 32 bit value. It is not Endian aware.

/*
    IO ROUTINES TO HANDLE IDX3 FILES
    IDX FILES ARE JUST PLAIN BINARY FILES WITH A FEW PADDED BYTES UPSTREAM OF THE DATA SECTION THAT STORES METADATA

    In idx3 pixel files (images), the pixel data is stored in the following format,

    [offset] [type]          [value]          [description]
    0000     32 bit integer  0x00000803(2051) magic number
    0004     32 bit integer  10000            number of images
    0008     32 bit integer  28               number of rows
    0012     32 bit integer  28               number of columns
    0016     unsigned byte   ??               pixel
    0017     unsigned byte   ??               pixel

    In idx1 label files, the label data is stored in the following format,

    [offset] [type]          [value]          [description]
    0000     32 bit integer  0x00000801(2049) magic number (MSB first)
    0004     32 bit integer  10000            number of items
    0008     unsigned byte   ??               label
    0009     unsigned byte   ??               label
    ........
    xxxx     unsigned byte   ??               label
    The labels values are 0 to 9.

*/

// IDX FILES USE BIG ENDIAN BYTE ORDERING FOR MULTIBYTE VALUES

// to store metadata of idx3 files
typedef struct idx3meta {
        uint32_t magic;
        uint32_t nimages;
        uint32_t nrows;
        uint32_t ncolumns;
} idx3meta_t;

// to store metadata of idx1 files
typedef struct idx1meta {
        uint32_t magic;
        uint32_t nlabels;
} idx1meta_t;

// expects an unzipped file
static inline uint8_t* open_idx(_In_ const wchar_t* const restrict file_name, _Out_ uint64_t* const nread_bytes) {
    *nread_bytes    = 0;
    HANDLE   handle = NULL;
    void*    buffer = NULL;
    uint32_t nbytes = 0;

    handle          = CreateFileW(file_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

    if (handle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER file_size;
        if (!GetFileSizeEx(handle, &file_size)) {
            fwprintf_s(stderr, L"Error %lu in GetFileSizeEx\n", GetLastError());
            return NULL;
        }

        // add an extra megabyte to the buffer, for safety.
        size_t buffsize = file_size.QuadPart + (1024U * 1024);

        // caller is responsible for freeing this buffer.
        buffer          = malloc(buffsize);
        if (buffer) {
            if (ReadFile(handle, buffer, buffsize, &nbytes, NULL)) {
                *nread_bytes = nbytes;
                return buffer;
            } else {
                fwprintf_s(stderr, L"Error %lu in ReadFile\n", GetLastError());
                CloseHandle(handle);
                free(buffer);
                return NULL;
            }
        } else {
            fputws(L"Memory allocation error: malloc returned NULL", stderr);
            CloseHandle(handle);
            return NULL;
        }
    } else {
        fwprintf_s(stderr, L"Error %lu in CreateFileW\n", GetLastError());
        return NULL;
    }
}

#endif //!__IDX3_H__