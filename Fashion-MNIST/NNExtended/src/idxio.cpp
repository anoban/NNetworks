#define _AMD64_ // architecture
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_MEAN

#include <cstdint>
#include <cstdio>

#include <errhandlingapi.h>
#include <fileapi.h>
#include <handleapi.h>
#include <heapapi.h>
#include <windef.h>
#include <winsock.h>

#include <idxio.hpp>

#pragma comment(lib, "Ws2_32.lib")  // ntohl()

// a generic file reading routine, that reads in an existing binary file and returns the buffer. (nullptr in case of a failure)
// returned memory needs to be freed using HeapFree()! NOT UCRT's free()
[[nodiscard]] static __forceinline uint8_t* open(_In_ const wchar_t* const file_name, _Inout_ size_t* const size) noexcept {
    uint8_t*       buffer    = nullptr;
    DWORD          nbytes    = 0UL;
    LARGE_INTEGER  liFsize   = { 0LLU };    // C++ doesn't support designated initializers
    const HANDLE64 hFile     = ::CreateFileW(file_name, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, nullptr);

    // process's default heap, non serializeable
    const HANDLE64 hProcHeap = ::GetProcessHeap();

    if (hFile == INVALID_HANDLE_VALUE) {
        ::fwprintf_s(stderr, L"Error %lu in CreateFileW\n", ::GetLastError());
        goto INVALID_HANDLE_ERR;
    }

    if (!hProcHeap) {
        ::fwprintf_s(stderr, L"Error %lu in GetProcessHeap\n", ::GetLastError());
        goto GET_FILESIZE_ERR;
    }

    if (!::GetFileSizeEx(hFile, &liFsize)) {
        ::fwprintf_s(stderr, L"Error %lu in GetFileSizeEx\n", ::GetLastError());
        goto GET_FILESIZE_ERR;
    }

    if (!(buffer = static_cast<uint8_t*>(::HeapAlloc(hProcHeap, 0UL, liFsize.QuadPart)))) {
        ::fwprintf_s(stderr, L"Error %lu in HeapAlloc\n", ::GetLastError());
        goto GET_FILESIZE_ERR;
    }

    if (!::ReadFile(hFile, buffer, liFsize.QuadPart, &nbytes, nullptr)) {
        ::fwprintf_s(stderr, L"Error %lu in ReadFile\n", ::GetLastError());
        goto READFILE_ERR;
    }

    ::CloseHandle(hFile);
    *size = nbytes;
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
[[nodiscard]] static __forceinline bool serialize(
    _In_ const wchar_t* const filename,
    _In_ const uint8_t* const buffer,
    _In_ const size_t         size,
    _In_ const bool           freebuffer /* specifies whether to free the buffer after serialization */
) noexcept {
    // buffer is assumed to be allocated with HeapAlloc, i.e HeapFree will be invoked to free the buffer NOT UCRT's free()
    // one major caveat is that the caller needs to pass in a byte stream instead of a image struct, which implies a potentially
    // unnecessary memory allocationand buffer creation from the image structs.
    // defining separate write routines for each image format will be redundant and will ruin the modularity of the project.

    const HANDLE64 hFile  = ::CreateFileW(filename, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    DWORD          nbytes = 0;

    if (hFile == INVALID_HANDLE_VALUE) {
        ::fwprintf_s(stderr, L"Error %4lu in CreateFileW\n", ::GetLastError());
        goto PREMATURE_RETURN;
    }

    if (!::WriteFile(hFile, buffer, size, &nbytes, nullptr)) {
        ::fwprintf_s(stderr, L"Error %4lu in WriteFile\n", ::GetLastError());
        goto PREMATURE_RETURN;
    }

    ::CloseHandle(hFile);
    if (freebuffer) {
        const HANDLE64 hProcHeap = ::GetProcessHeap(); // WARNING :: ignoring potential errors here
        ::HeapFree(hProcHeap, 0, reinterpret_cast<LPVOID>(const_cast<uint8_t*>(buffer)));
    }
    
    return true;

PREMATURE_RETURN:
    ::CloseHandle(hFile);
    return false;
}

// BEGIN IDX1

// constructor of idx1 class
idxio::idx1::idx1(_In_ const wchar_t* const filename) noexcept {
    size_t     fsize {};
    // open will report errors, if any were encountered, caller doesn't need to
    const auto filebuffer { ::open(filename, &fsize) };
    if (filebuffer) {
        idxmagic = ::ntohl(*reinterpret_cast<uint32_t*>(filebuffer));
        nlabels  = ::ntohl(*reinterpret_cast<uint32_t*>(filebuffer + 4));
        buffer   = filebuffer;
        labels   = filebuffer + 8;
        usable   = true;
    }
    // else leave other member variables in their default initialized state
    return;
}

// destructor of idx1
idxio::idx1::~idx1(void) noexcept {
    const HANDLE64 hProcHeap { ::GetProcessHeap() };
    if (hProcHeap == INVALID_HANDLE_VALUE) {
        ::fwprintf_s(stderr, L"Error %lu in GetProcessHeap\n", ::GetLastError());
        return;
    }

    if (!::HeapFree(hProcHeap, 0, buffer)) {
        ::fwprintf_s(stderr, L"Error %lu in HeapFree\n", ::GetLastError());
        return;
    }

    idxmagic = nlabels = 0;
    buffer = labels = nullptr;
    usable          = false;
    return;
}


bool idxio::idx1::is_usable(void) const noexcept { return usable; }

size_t idxio::idx1::size(void) const noexcept { return nlabels; }

idxio::idx1::const_iterator idxio::idx1::cbegin(void) const noexcept { return labels; }

idxio::idx1::const_iterator idxio::idx1::cend(void) const noexcept { return labels + nlabels; }

// END IDX1


// BEGIN IDX3

// constructor of idx3 class
idxio::idx3::idx3(_In_ const wchar_t* const filename) noexcept {
    size_t     fsize {};
    // open will report errors, if any were encountered, caller doesn't need to
    const auto filebuffer { ::open(filename, &fsize) };
    if (filebuffer) {
        idxmagic = ::ntohl(*reinterpret_cast<uint32_t*>(filebuffer));
        nimages  = ::ntohl(*reinterpret_cast<uint32_t*>(filebuffer + 4));
        nrows_perimage = ::ntohl(*reinterpret_cast<uint32_t*>(filebuffer + 8));
        ncols_perimage = ::ntohl(*reinterpret_cast<uint32_t*>(filebuffer + 12));
        buffer   = filebuffer;
        pixels   = filebuffer + 16;
        usable   = true;
    }
    // else leave other member variables in their default initialized state
    return;
}

// destructor of idx3
idxio::idx3::~idx3(void) noexcept {
    const HANDLE64 hProcHeap { ::GetProcessHeap() };
    if (hProcHeap == INVALID_HANDLE_VALUE) {
        ::fwprintf_s(stderr, L"Error %lu in GetProcessHeap\n", ::GetLastError());
        return;
    }

    if (!::HeapFree(hProcHeap, 0, buffer)) {
        ::fwprintf_s(stderr, L"Error %lu in HeapFree\n", ::GetLastError());
        return;
    }

    idxmagic = nimages = nrows_perimage = ncols_perimage = 0;
    buffer = pixels = nullptr;
    usable          = false;
    return;
}

bool                        idxio::idx3::is_usable(void) const noexcept { return usable; }

size_t                      idxio::idx3::size(void) const noexcept { return nimages; }

idxio::idx3::const_iterator idxio::idx3::cbegin(void) const noexcept { return pixels; }

idxio::idx3::const_iterator idxio::idx3::cend(void) const noexcept { return pixels + (nimages * nrows_perimage * ncols_perimage); }

std::pair<size_t, size_t>   idxio::idx3::shape(void) const noexcept { return std::make_pair<size_t, size_t>(nrows_perimage, ncols_perimage); }

// END IDX3