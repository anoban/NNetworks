#include <stdexcept>

#include <idxio.hpp>

namespace helpers {

    [[nodiscard]] static inline std::optional<std::vector<uint8_t>> __cdecl open(
        _In_ const wchar_t* const filename, _Inout_ unsigned long* const size
    ) noexcept {
        *size = 0;
        std::vector<uint8_t> buffer {};
        LARGE_INTEGER        liFsize { .QuadPart = 0LLU };
        const HANDLE64       hFile = ::CreateFileW(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

        if (hFile == INVALID_HANDLE_VALUE) {
            ::fwprintf_s(stderr, L"Error %lu in CreateFileW\n", ::GetLastError());
            goto INVALID_HANDLE_ERR;
        }

        if (!::GetFileSizeEx(hFile, &liFsize)) {
            ::fwprintf_s(stderr, L"Error %lu in GetFileSizeEx\n", ::GetLastError());
            goto GET_FILESIZE_ERR;
        }

        buffer.resize(liFsize.QuadPart);

        if (!::ReadFile(hFile, buffer.data(), liFsize.QuadPart, size, NULL)) {
            ::fwprintf_s(stderr, L"Error %lu in ReadFile\n", ::GetLastError());
            goto INVALID_HANDLE_ERR;
        }

        ::CloseHandle(hFile);
        return buffer;

GET_FILESIZE_ERR:
        ::CloseHandle(hFile);
INVALID_HANDLE_ERR:
        return std::nullopt;
    }

    template<typename T>
    [[nodiscard]] static inline bool __cdecl serialize(
        _In_ const wchar_t* const filename,
        _In_ const std::vector<T>& buffer,
        _In_opt_ const typename std::enable_if<std::is_arithmetic_v<T>, bool>::type = true
        // maybe a requires clause will be better instead of SFINAE here!
    ) noexcept {
        const HANDLE64 hFile  = ::CreateFileW(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD          nbytes = 0;

        if (hFile == INVALID_HANDLE_VALUE) {
            ::fwprintf_s(stderr, L"Error %4lu in CreateFileW\n", ::GetLastError());
            return false; // do not call CloseHandle on an invalid handle!!!
        }

        if (!WriteFile(hFile, reinterpret_cast<const void*>(buffer.data()), buffer.size() * sizeof(T), &nbytes, NULL)) {
            ::fwprintf_s(stderr, L"Error %4lu in WriteFile\n", ::GetLastError());
            goto PREMATURE_RETURN;
        }

        ::CloseHandle(hFile);
        return true;

PREMATURE_RETURN:
        ::CloseHandle(hFile);
        return false;
    }

} // namespace helpers

////////////////////////////////////////////////
// class idx1 member function implementations //
////////////////////////////////////////////////

constexpr idx::idx1::idx1() noexcept { }

constexpr idx::idx1::idx1(_In_ const wchar_t* const filename) { }

constexpr idx::idx1::idx1(_In_ const idx1& other) noexcept { }

constexpr idx::idx1::idx1(_In_ idx1&& other) noexcept { }

constexpr idx::idx1& idx::idx1::operator=(_In_ const idx1& other) noexcept { }

constexpr idx::idx1& idx::idx1::operator=(_In_ idx1&& other) noexcept { }

constexpr idx::idx1::~idx1() noexcept { }

template<typename char_t> std::basic_ostream<char_t>& operator<<(std::basic_ostream<char_t>& ostr, const idx::idx1& object) { }

////////////////////////////////////////////////
// class idx3 member function implementations //
////////////////////////////////////////////////

constexpr idx::idx3::idx3() noexcept : idxmagic {}, nimages {}, nrows_perimage {}, ncols_perimage {}, buffer {}, pixels {} { }

constexpr idx::idx3::idx3(_In_ const wchar_t* const filename) {
    unsigned long fsize {};
    const auto    filebuffer = helpers::open(filename, &fsize);
    if (!filebuffer.has_value()) throw utilities::nnext_error { "" };

    idxmagic       = ::ntohl(*(uint32_t*) (filebuffer));
    nimages        = ::ntohl(*(uint32_t*) (filebuffer + 4));
    nrows_perimage = ::ntohl(*(uint32_t*) (filebuffer + 8));
    ncols_perimage = ::ntohl(*(uint32_t*) (filebuffer + 12));
    buffer         = filebuffer;
    pixels         = filebuffer + 16;
}

constexpr idx::idx3::idx3(_In_ const idx3& other) noexcept { }

constexpr idx::idx3::idx3(_In_ idx3&& other) noexcept { }

constexpr idx::idx3& idx::idx3::operator=(_In_ const idx3& other) noexcept { }

constexpr idx::idx3& idx::idx3::operator=(_In_ idx3&& other) noexcept { }

constexpr idx::idx3::~idx3() noexcept { }

template<typename char_t> std::basic_ostream<char_t>& operator<<(std::basic_ostream<char_t>& ostr, const idx::idx3& object) { }

idx1_t OpenIdx1(_In_ const wchar_t* const filename) {
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
