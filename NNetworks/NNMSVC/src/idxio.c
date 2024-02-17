#define _AMD64_
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_MEAN
#include <errhandlingapi.h>
#include <fileapi.h>
#include <handleapi.h>
#include <idxio.h>
#include <stdio.h>
#include <stdlib.h>
#include <windef.h>
#include <winsock.h>               // htonl

#pragma comment(lib, "Ws2_32.lib") // htonl

idx1_t openidx1(_In_ const wchar_t* const restrict file_name) {
    uint8_t *     buffer = NULL, *labels = NULL;
    DWORD         nbytes  = 0;
    LARGE_INTEGER liFsize = { .QuadPart = 0LLU };
    idx1_t        result  = { .magic = 0U, .nlabels = 0U, .data = NULL };

    const HANDLE64 hFile  = CreateFileW(file_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        fwprintf_s(stderr, L"Error %lu in CreateFileW\n", GetLastError());
        goto INVHANDLE_ERR;
    }

    if (!GetFileSizeEx(hFile, &liFsize)) {
        fwprintf_s(stderr, L"Error %lu in GetFileSizeEx\n", GetLastError());
        goto GETFSIZE_ERR;
    }

    if (!(buffer = malloc(liFsize.QuadPart))) {
        fputws(L"Memory allocation error: malloc returned NULL", stderr);
        goto GETFSIZE_ERR;
    }

    if (!ReadFile(hFile, buffer, liFsize.QuadPart, &nbytes, NULL)) {
        fwprintf_s(stderr, L"Error %lu in ReadFile\n", GetLastError());
        goto READFILE_ERR;
    }

    const uint32_t magic   = htonl(*((uint32_t*) (buffer)));     // first 4 bytes
    const uint32_t nlabels = htonl(*((uint32_t*) (buffer + 4))); // next 4 bytes

    if (!(labels = malloc(liFsize.QuadPart - 8))) {
        fputws(L"Memory allocation error: malloc returned NULL", stderr);
        goto READFILE_ERR;
    }

    memcpy_s(labels, liFsize.QuadPart - 8, buffer + 8, liFsize.QuadPart - 8);
    result.data    = labels;
    result.magic   = magic;
    result.nlabels = nlabels;

READFILE_ERR:
    free(buffer);
GETFSIZE_ERR:
    CloseHandle(hFile);
INVHANDLE_ERR:
    return result;
}

idx3_t openidx3(_In_ const wchar_t* const restrict file_name) {
    uint8_t *     buffer = NULL, *labels = NULL;
    DWORD         nbytes  = 0;
    LARGE_INTEGER liFsize = { .QuadPart = 0LLU };
    idx3_t        result  = { .magic = 0U, .nimages = 0U, .nrows_img = 0U, .ncols_img = 0U, .data = NULL };

    const HANDLE64 hFile  = CreateFileW(file_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        fwprintf_s(stderr, L"Error %lu in CreateFileW\n", GetLastError());
        goto INVHANDLE_ERR;
    }

    if (!GetFileSizeEx(hFile, &liFsize)) {
        fwprintf_s(stderr, L"Error %lu in GetFileSizeEx\n", GetLastError());
        goto GETFSIZE_ERR;
    }

    if (!(buffer = malloc(liFsize.QuadPart))) {
        fputws(L"Memory allocation error: malloc returned NULL", stderr);
        goto GETFSIZE_ERR;
    }

    if (!ReadFile(hFile, buffer, liFsize.QuadPart, &nbytes, NULL)) {
        fwprintf_s(stderr, L"Error %lu in ReadFile\n", GetLastError());
        goto READFILE_ERR;
    }

    const uint32_t magic   = htonl(*((uint32_t*) (buffer)));      // first 4 bytes
    const uint32_t nimages = htonl(*((uint32_t*) (buffer + 4)));  // second 4 bytes
    const uint32_t nrows   = htonl(*((uint32_t*) (buffer + 8)));  // third 4 bytes
    const uint32_t ncols   = htonl(*((uint32_t*) (buffer + 12))); // fourth 4 bytes

    if (!(labels = malloc(liFsize.QuadPart - 16))) {
        fputws(L"Memory allocation error: malloc returned NULL", stderr);
        goto READFILE_ERR;
    }

    memcpy_s(labels, liFsize.QuadPart - 16, buffer + 16, liFsize.QuadPart - 16);
    result.data      = labels;
    result.magic     = magic;
    result.nimages   = nimages;
    result.nrows_img = nrows;
    result.ncols_img = ncols;

READFILE_ERR:
    free(buffer);
GETFSIZE_ERR:
    CloseHandle(hFile);
INVHANDLE_ERR:
    return result;
}