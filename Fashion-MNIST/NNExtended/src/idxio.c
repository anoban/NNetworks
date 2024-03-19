#define _AMD64_ // architecture
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_MEAN

#include <errhandlingapi.h>
#include <fileapi.h>
#include <handleapi.h>
#include <heapapi.h>
#include <idxio.h>
#include <stdint.h>
#include <stdio.h>
#include <windef.h>
#include <winsock.h>

#pragma comment(lib, "Ws2_32.lib") // ntohl()

// a generic file reading routine, that reads in an existing binary file and returns the buffer. (NULL in case of a failure)
// returned memory needs to be freed using HeapFree()! NOT UCRT's free()
static inline uint8_t* open(_In_ const wchar_t* const restrict file_name, _Inout_ size_t* const size) {
    uint8_t*       buffer    = NULL;
    DWORD          nbytes    = 0UL;
    LARGE_INTEGER  liFsize   = { .QuadPart = 0LLU };
    const HANDLE64 hFile     = CreateFileW(file_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

    // process's default heap, non serializeable
    const HANDLE64 hProcHeap = GetProcessHeap();

    if (hFile == INVALID_HANDLE_VALUE) {
        fwprintf_s(stderr, L"Error %lu in CreateFileW\n", GetLastError());
        goto INVALID_HANDLE_ERR;
    }

    if (!hProcHeap) {
        fwprintf_s(stderr, L"Error %lu in GetProcessHeap\n", GetLastError());
        goto GET_FILESIZE_ERR;
    }

    if (!GetFileSizeEx(hFile, &liFsize)) {
        fwprintf_s(stderr, L"Error %lu in GetFileSizeEx\n", GetLastError());
        goto GET_FILESIZE_ERR;
    }

    if (!(buffer = HeapAlloc(hProcHeap, 0UL, liFsize.QuadPart))) {
        fwprintf_s(stderr, L"Error %lu in HeapAlloc\n", GetLastError());
        goto GET_FILESIZE_ERR;
    }

    if (!ReadFile(hFile, buffer, liFsize.QuadPart, &nbytes, NULL)) {
        fwprintf_s(stderr, L"Error %lu in ReadFile\n", GetLastError());
        goto READFILE_ERR;
    }

    CloseHandle(hFile);
    *size = nbytes;
    return buffer;

READFILE_ERR:
    HeapFree(hProcHeap, 0UL, buffer);
GET_FILESIZE_ERR:
    CloseHandle(hFile);
INVALID_HANDLE_ERR:
    *size = 0;
    return NULL;
}

// a file format agnostic write routine to serialize binary image files.
// if a file with the specified name exists on disk, it will be overwritten.
static inline bool serialize(
    _In_ const wchar_t* const restrict filename,
    _In_ const uint8_t* const restrict buffer,
    _In_ const size_t size,
    _In_ const bool   freebuffer /* specifies whether to free the buffer after serialization */
) {
    // buffer is assumed to be allocated with HeapAlloc, i.e HeapFree will be invoked to free the buffer NOT UCRT's free()
    // one major caveat is that the caller needs to pass in a byte stream instead of a image struct, which implies a potentially
    // unnecessary memory allocationand buffer creation from the image structs.
    // defining separate write routines for each image format will be redundant and will ruin the modularity of the project.

    const HANDLE64 hFile  = CreateFileW(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD          nbytes = 0;

    if (hFile == INVALID_HANDLE_VALUE) {
        fwprintf_s(stderr, L"Error %4lu in CreateFileW\n", GetLastError());
        goto PREMATURE_RETURN;
    }

    if (!WriteFile(hFile, buffer, size, &nbytes, NULL)) {
        fwprintf_s(stderr, L"Error %4lu in WriteFile\n", GetLastError());
        goto PREMATURE_RETURN;
    }

    CloseHandle(hFile);
    if (freebuffer) {
        const HANDLE64 hProcHeap = GetProcessHeap(); // WARNING  ignoring potential errors here
        HeapFree(hProcHeap, 0, buffer);
    }

    return true;

PREMATURE_RETURN:
    CloseHandle(hFile);
    return false;
}

idx1_t OpenIdx1(_In_ const wchar_t* const filename) {
    size_t     fsize {};
    // open will report errors, if any were encountered, caller doesn't need to
    const auto filebuffer { open(filename, &fsize) };
    if (filebuffer) {
        idxmagic = ntohl(*reinterpret_cast<uint32_t*>(filebuffer));
        nlabels  = ntohl(*reinterpret_cast<uint32_t*>(filebuffer + 4));
        buffer   = filebuffer;
        labels   = filebuffer + 8;
        usable   = true;
    }
    // else leave other member variables in their default initialized state
    return;
}

bool FreeIdx1(void) {
    const HANDLE64 hProcHeap { GetProcessHeap() };
    if (hProcHeap == INVALID_HANDLE_VALUE) {
        fwprintf_s(stderr, L"Error %lu in GetProcessHeap\n", GetLastError());
        return;
    }

    if (!HeapFree(hProcHeap, 0, buffer)) {
        fwprintf_s(stderr, L"Error %lu in HeapFree\n", GetLastError());
        return;
    }

    idxmagic = nlabels = 0;
    buffer = labels = NULL;
    usable          = false;
    return;
}

idx3_t OpenIdx3(_In_ const wchar_t* const filename) {
    size_t     fsize {};
    // open will report errors, if any were encountered, caller doesn't need to
    const auto filebuffer { open(filename, &fsize) };
    if (filebuffer) {
        idxmagic       = ntohl(*reinterpret_cast<uint32_t*>(filebuffer));
        nimages        = ntohl(*reinterpret_cast<uint32_t*>(filebuffer + 4));
        nrows_perimage = ntohl(*reinterpret_cast<uint32_t*>(filebuffer + 8));
        ncols_perimage = ntohl(*reinterpret_cast<uint32_t*>(filebuffer + 12));
        buffer         = filebuffer;
        pixels         = filebuffer + 16;
        usable         = true;
    }
    // else leave other member variables in their default initialized state
    return;
}

bool FreeIdx3(idx3_t object) {
    const HANDLE64 hProcHeap { GetProcessHeap() };
    if (hProcHeap == INVALID_HANDLE_VALUE) {
        fwprintf_s(stderr, L"Error %lu in GetProcessHeap\n", GetLastError());
        return;
    }

    if (!HeapFree(hProcHeap, 0, buffer)) {
        fwprintf_s(stderr, L"Error %lu in HeapFree\n", GetLastError());
        return;
    }

    idxmagic = nimages = nrows_perimage = ncols_perimage = 0;
    buffer = pixels = NULL;
    usable          = false;
    return;
}
