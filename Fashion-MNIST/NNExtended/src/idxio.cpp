#include <idxio.hpp>

namespace helpers {

    // a generic file reading routine, that reads in an existing binary file and returns the buffer. (NULL in case of a failure)
    // returned memory needs to be freed using HeapFree()! NOT UCRT's free()
    static inline std::optional<std::vector<uint8_t>> open(_In_ const wchar_t* const file_name, _Inout_ size_t* const size) {
        std::vector<uint8_t> buffer {};
        DWORD                nbytes {};
        LARGE_INTEGER        liFsize { .QuadPart = 0LLU };
        const HANDLE64       hFile     = ::CreateFileW(file_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

        // process's default heap, non serializeable
        const HANDLE64       hProcHeap = ::GetProcessHeap();

        if (hFile == INVALID_HANDLE_VALUE) {
            ::fwprintf_s(stderr, L"Error %lu in CreateFileW\n", GetLastError());
            goto INVALID_HANDLE_ERR;
        }

        if (!hProcHeap) {
            ::fwprintf_s(stderr, L"Error %lu in GetProcessHeap\n", GetLastError());
            goto GET_FILESIZE_ERR;
        }

        if (!GetFileSizeEx(hFile, &liFsize)) {
            ::fwprintf_s(stderr, L"Error %lu in GetFileSizeEx\n", GetLastError());
            goto GET_FILESIZE_ERR;
        }

        if (!(buffer = HeapAlloc(hProcHeap, 0UL, liFsize.QuadPart))) {
            ::fwprintf_s(stderr, L"Error %lu in HeapAlloc\n", GetLastError());
            goto GET_FILESIZE_ERR;
        }

        if (!ReadFile(hFile, buffer, liFsize.QuadPart, &nbytes, NULL)) {
            ::fwprintf_s(stderr, L"Error %lu in ReadFile\n", GetLastError());
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
        return std::nullopt;
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

} // namespace helpers

constexpr idx::idx1::idx1() noexcept { }

constexpr idx::idx1::idx1(_In_ const wchar_t* const filename) noexcept { }

constexpr idx::idx1::idx1(_In_ const idx1& other) noexcept { }

constexpr idx::idx1::idx1(_In_ idx1&& other) noexcept { }

constexpr idx::idx1& idx::idx1::operator=(_In_ const idx::idx1& other) noexcept { }

constexpr idx::idx1& idx::idx1::operator=(_In_ idx::idx1&& other) noexcept { }

constexpr idx::idx1::~idx1() noexcept { }

template<typename char_t> std::basic_ostream<char_t>& operator<<(std::basic_ostream<char_t>& ostr, const idx::idx1& object) { }

idx1_t                                                OpenIdx1(_In_ const wchar_t* const filename) {
    size_t               fsize      = 0;
    idx1_t               tmp        = { 0 };
    // open() will report errors, if any were encountered, caller doesn't need to
    const uint8_t* const filebuffer = open(filename, &fsize);
    if (filebuffer) {
        tmp.idxmagic  = ntohl(*(uint32_t*) (filebuffer));
        tmp.nlabels   = ntohl(*(uint32_t*) (filebuffer + 4));
        tmp.buffer    = filebuffer;
        tmp.labels    = filebuffer + 8;
        tmp.is_usable = true;
        return tmp;
    }
    // else leave other member variables in their default initialized state
    return tmp;
}

bool FreeIdx1(idx1_t* const restrict object) {
    const HANDLE64 hProcHeap = GetProcessHeap();
    if (hProcHeap == INVALID_HANDLE_VALUE) {
        fwprintf_s(stderr, L"Error %lu in GetProcessHeap\n", GetLastError());
        return false;
    }

    if (!HeapFree(hProcHeap, 0, object->buffer)) {
        fwprintf_s(stderr, L"Error %lu in HeapFree\n", GetLastError());
        return false;
    }

    object->idxmagic = object->nlabels = 0;
    object->buffer = object->labels = NULL;
    object->is_usable               = false;
    return true;
}

idx3_t OpenIdx3(_In_ const wchar_t* const filename) {
    size_t               fsize      = 0;
    idx3_t               tmp        = { 0 };
    // open will report errors, if any were encountered, caller doesn't need to
    const uint8_t* const filebuffer = open(filename, &fsize);
    if (filebuffer) {
        tmp.idxmagic       = ntohl(*(uint32_t*) (filebuffer));
        tmp.nimages        = ntohl(*(uint32_t*) (filebuffer + 4));
        tmp.nrows_perimage = ntohl(*(uint32_t*) (filebuffer + 8));
        tmp.ncols_perimage = ntohl(*(uint32_t*) (filebuffer + 12));
        tmp.buffer         = filebuffer;
        tmp.pixels         = filebuffer + 16;
        tmp.is_usable      = true;
        return tmp;
    }
    // else leave other member variables in their default initialized state
    return tmp;
}

bool FreeIdx3(idx3_t* const restrict object) {
    const HANDLE64 hProcHeap = GetProcessHeap();
    if (hProcHeap == INVALID_HANDLE_VALUE) {
        fwprintf_s(stderr, L"Error %lu in GetProcessHeap\n", GetLastError());
        return false;
    }

    if (!HeapFree(hProcHeap, 0, object->buffer)) {
        fwprintf_s(stderr, L"Error %lu in HeapFree\n", GetLastError());
        return false;
    }

    object->idxmagic = object->nimages = object->nrows_perimage = object->ncols_perimage = 0;
    object->buffer = object->pixels = NULL;
    object->is_usable               = false;
    return true;
}

void PrintIdx1(_In_ const idx1_t* const object) {
    wprintf_s(L"Idx1 object ::>\nmagic       :: %08X\nlabel count :: %6u\n\n", object->idxmagic, object->nlabels);
    return;
}

void PrintIdx3(_In_ const idx3_t* const object) {
    wprintf_s(
        L"Idx3 object ::>\nmagic        :: %08X\nimage count  :: %6u\nimage height :: %6u\nimage width  :: %6u\n\n",
        object->idxmagic,
        object->nimages,
        object->nrows_perimage,
        object->ncols_perimage
    );
    return;
}
