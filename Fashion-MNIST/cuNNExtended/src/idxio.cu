#define _AMD64_ // architecture
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_MEAN
#include <cstdbool>
#include <cstdint>
#include <cstdio>
#include <vector>

#include <errhandlingapi.h>
#include <fileapi.h>
#include <handleapi.h>
#include <heapapi.h>
#include <windef.h>

// a generic file reading routine, that reads in an existing binary file and returns the buffer. (NULL in case of a failure)
// returned memory needs to be freed using HeapFree()! NOT UCRT's free()
static inline uint8_t* open(_In_ const wchar_t* const file_name, _Inout_ size_t* const size) {
    uint8_t*       buffer    = nullptr;
    DWORD          nbytes    = 0UL;
    LARGE_INTEGER  liFsize   = { .QuadPart = 0LLU };
    const HANDLE64 hFile     = ::CreateFileW(file_name, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, nullptr);

    // process's default heap, non serializeable
    const HANDLE64 hProcHeap = ::GetProcessHeap();

    if (hFile == INVALID_HANDLE_VALUE) {
        fwprintf_s(stderr, L"Error %lu in CreateFileW\n", ::GetLastError());
        goto INVALID_HANDLE_ERR;
    }

    if (!hProcHeap) {
        fwprintf_s(stderr, L"Error %lu in GetProcessHeap\n", ::GetLastError());
        goto GET_FILESIZE_ERR;
    }

    if (!::GetFileSizeEx(hFile, &liFsize)) {
        fwprintf_s(stderr, L"Error %lu in GetFileSizeEx\n", ::GetLastError());
        goto GET_FILESIZE_ERR;
    }

    if (!(buffer = static_cast<uint8_t*>(::HeapAlloc(hProcHeap, 0UL, liFsize.QuadPart)))) {
        fwprintf_s(stderr, L"Error %lu in HeapAlloc\n", ::GetLastError());
        goto GET_FILESIZE_ERR;
    }

    if (!ReadFile(hFile, buffer, liFsize.QuadPart, &nbytes, nullptr)) {
        fwprintf_s(stderr, L"Error %lu in ReadFile\n", ::GetLastError());
        goto READFILE_ERR;
    }

    ::CloseHandle(hFile);
    *size = liFsize.QuadPart;
    return buffer;

READFILE_ERR:
    ::HeapFree(hProcHeap, 0UL, buffer);
GET_FILESIZE_ERR:
    ::CloseHandle(hFile);
INVALID_HANDLE_ERR:
    *size = 0;
    return nullptr;
}

// a file format agnostic write routine to serialize binary image files.
// if a file with the specified name exists on disk, it will be overwritten.
static inline bool serialize(
    _In_ const wchar_t* const filename,
    _In_ const uint8_t* const buffer,
    _In_ const size_t         size,
    _In_ const bool           freebuffer /* specifies whether to free the buffer after serialization */
) {
    // buffer is assumed to be allocated with HeapAlloc, i.e HeapFree will be invoked to free the buffer NOT UCRT's free()
    // one major caveat is that the caller needs to pass in a byte stream instead of a image struct, which implies a potentially
    // unnecessary memory allocationand buffer creation from the image structs.
    // defining separate write routines for each image format will be redundant and will ruin the modularity of the project.

    const HANDLE64 hFile  = ::CreateFileW(filename, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    DWORD          nbytes = 0;

    if (hFile == INVALID_HANDLE_VALUE) {
        fwprintf_s(stderr, L"Error %4lu in CreateFileW\n", ::GetLastError());
        goto PREMATURE_RETURN;
    }

    if (!WriteFile(hFile, buffer, size, &nbytes, nullptr)) {
        fwprintf_s(stderr, L"Error %4lu in WriteFile\n", ::GetLastError());
        goto PREMATURE_RETURN;
    }

    ::CloseHandle(hFile);
    if (freebuffer) {
        const HANDLE64 hProcHeap = GetProcessHeap(); // WARNING :: ignoring potential errors here
        ::HeapFree(hProcHeap, 0, buffer);
    }
    ::CloseHandle(hFile);
    return true;

PREMATURE_RETURN:
    ::CloseHandle(hFile);
    return false;
}
