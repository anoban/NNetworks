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
#include <winsock.h>

idx1_t openidx1(_In_ const wchar_t* const restrict file_name) {
    HANDLE64*     hFile   = NULL;
    void*         buffer  = NULL;
    DWORD         nbytes  = 0;
    LARGE_INTEGER liFsize = { .QuadPart = 0LLU };

    hFile                 = CreateFileW(file_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

    if (hFile != INVALID_HANDLE_VALUE) {
        // if not able to decide the size of the file, exit the routine.
        if (!GetFileSizeEx(hFile, &liFsize)) {
            fwprintf_s(stderr, L"Error %lu in GetFileSizeEx\n", GetLastError());
            return (idx1_t) { .magic = 0U, .nlabels = 0, .data = NULL };
        }

        buffer = malloc(liFsize.QuadPart);
        if (buffer) {
            if (ReadFile(hFile, buffer, liFsize.QuadPart, &nbytes, NULL)) {
                // implement idx1 parsing
                idx1_t temp = { 0 };
                temp.magic  = htonl(*(int32_t*) (buffer));

                return temp;
            } else {
                fwprintf_s(stderr, L"Error %lu in ReadFile\n", GetLastError());
                CloseHandle(hFile);
                free(buffer);
                return (idx1_t) { .magic = 0U, .nlabels = 0, .data = NULL };
            }
        } else {
            fputws(L"Memory allocation error: malloc returned NULL", stderr);
            CloseHandle(hFile);
            return (idx1_t) { .magic = 0U, .nlabels = 0, .data = NULL };
        }
    } else {
        fwprintf_s(stderr, L"Error %lu in CreateFileW\n", GetLastError());
        return (idx1_t) { .magic = 0U, .nlabels = 0, .data = NULL };
    }
}

idx3_t openidx3(_In_ const wchar_t* const restrict file_name) {
    *nread_bytes    = 0;
    HANDLE64 hFile  = NULL;
    void*    buffer = NULL;
    uint32_t nbytes = 0;

    hFile           = CreateFileW(file_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

    if (hFile != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER liFsize;
        if (!GetFileSizeEx(hFile, &liFsize)) {
            fwprintf_s(stderr, L"Error %lu in GetFileSizeEx\n", GetLastError());
            return NULL;
        }

        // add an extra megabyte to the buffer, for safety.
        size_t buffsize = liFsize.QuadPart + (1024U * 1024);

        // caller is responsible for freeing this buffer.
        buffer          = malloc(buffsize);
        if (buffer) {
            if (ReadFile(hFile, buffer, buffsize, &nbytes, NULL)) {
                *nread_bytes = nbytes;
                return buffer;
            } else {
                fwprintf_s(stderr, L"Error %lu in ReadFile\n", GetLastError());
                CloseHandle(hFile);
                free(buffer);
                return NULL;
            }
        } else {
            fputws(L"Memory allocation error: malloc returned NULL", stderr);
            CloseHandle(hFile);
            return NULL;
        }
    } else {
        fwprintf_s(stderr, L"Error %lu in CreateFileW\n", GetLastError());
        return NULL;
    }
}