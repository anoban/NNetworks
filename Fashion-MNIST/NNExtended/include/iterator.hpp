#include <cassert>
#include <cstdint>
#include <iterator>
#include <type_traits>

template<typename T> class random_access_iterator final { // unchecked random access iterator
        // if invalid memory access happens, the OS may raise an access violation exception, the iterator won't do anything about this in release mode
        // in debug mode, certain preventative asserts may fail, indicating where things went wrong

    public:
        using value_type        = typename std::remove_cv<T>::type;
        using pointer           = T*;
        // T* preserves const correctness if T is a const qualified type, value_type* would have removed that const qualifier
        using const_pointer     = const value_type*;
        using reference         = T&; // T& instead of value_type& for the sake of const correctness
        using const_reference   = const value_type&;
        using difference_type   = signed long long;   // aka ptrdiff_t
        using size_type         = unsigned long long; // aka size_t
        using iterator_category = std::random_access_iterator_tag;

        // clang-format off
#if !defined(_DEBUG) && !defined(__TEST__)    // for testing purposes make these members public!
    private:
#endif 
        // clang-format on             
        
        // using an unqualified pointer here will raise errors with const iterables!
        pointer   _rsrc;   // pointer to the iterable resource
        size_type _length; // number of elements in the iterable
        size_type _offset; // current position in the iterable

    public:
        // will require an explicit template type specification
        constexpr inline __cdecl random_access_iterator() noexcept : _rsrc(), _length(), _offset() { }

        // resource pointer and length of the iterable
        constexpr inline __cdecl random_access_iterator(_In_ T* const _res, _In_ const size_t& _sz) noexcept :
            _rsrc(_res), _length(_sz), _offset() { }

        // resource pointer, length of the iterable and the current iterator position, will need this for .end() methods
        constexpr inline __cdecl random_access_iterator(_In_ T* const _res, _In_ const size_t& _sz, _In_ const size_t& _pos) noexcept :
            _rsrc(_res), _length(_sz), _offset(_pos) {
            assert(_sz >= _pos);
        }

        constexpr inline __cdecl random_access_iterator(_In_ const random_access_iterator& _other) noexcept :
            _rsrc(_other._rsrc), _length(_other._length), _offset(_other._offset) { }

        constexpr inline __cdecl random_access_iterator(_In_ random_access_iterator&& _other) noexcept :
            _rsrc(_other._rsrc), _length(_other._length), _offset(_other._offset) {
            // cleanup the stolen from resource
            _other._rsrc   = nullptr;
            _other._length = _other._offset = 0;
        }

        constexpr inline random_access_iterator& __cdecl operator=(_In_ const random_access_iterator& _other) noexcept {
            if (this == &_other) return *this;
            _rsrc   = _other._rsrc;
            _length = _other._length;
            _offset = _other._offset;
            return *this;
        }

        constexpr inline random_access_iterator& __cdecl operator=(_In_ random_access_iterator&& _other) noexcept {
            if (this == &_other) return *this;
            _rsrc          = _other._rsrc;
            _length        = _other._length;
            _offset        = _other._offset;

            // cleanup
            _other._rsrc   = nullptr;
            _other._length = _other._offset = 0;
            return *this;
        }

        constexpr inline __cdecl ~random_access_iterator() noexcept {
            _rsrc   = nullptr;
            _length = _offset = 0;
        }

        // using the __stdcall calling convention to very frequently invoked methods!

        [[nodiscard]] constexpr inline reference __stdcall operator*() noexcept { return _rsrc[_offset]; }

        [[nodiscard]] constexpr inline const_reference __stdcall operator*() const noexcept { return _rsrc[_offset]; }

        [[nodiscard]] constexpr inline pointer __cdecl _Unwrapped() noexcept { return _rsrc; }

        [[nodiscard]] constexpr inline const_pointer __cdecl _Unwrapped() const noexcept { return _rsrc; }

        constexpr inline random_access_iterator& __stdcall operator++() noexcept {
            _offset++;
            assert(_offset <= _length);
            return *this;
        }

        [[nodiscard]] constexpr inline random_access_iterator __stdcall operator++(int) noexcept {
            _offset++;
            assert(_offset <= _length);
            return { _rsrc, _length, _offset - 1 };
        }

        constexpr inline random_access_iterator& __stdcall operator--() noexcept {
            _offset--;
            assert(_offset <= _length); // assert(_offset >= 0); won't help because _offset is unsigned, so instead check for wraparounds
            return *this;
        }

        [[nodiscard]] constexpr inline random_access_iterator __stdcall operator--(int) noexcept {
            _offset--;
            assert(_offset <= _length); // assert(_offset >= 0); won't help because _offset is unsigned, so instead check for wraparounds
            return { _rsrc, _length, _offset + 1 };
        }

        [[nodiscard]] constexpr inline bool __stdcall operator==(const random_access_iterator& _other) noexcept {
            return _rsrc == _other._rsrc && _offset == _other._offset;
        }

        [[nodiscard]] constexpr inline bool __stdcall operator!=(const random_access_iterator& _other) noexcept {
            return _rsrc != _other._rsrc || _offset != _other._offset;
        }

        [[nodiscard]] constexpr inline bool __stdcall operator<(const random_access_iterator& _other) noexcept {
            return _rsrc == _other._rsrc && _offset < _other._offset;
        }

        [[nodiscard]] constexpr inline bool __stdcall operator<=(const random_access_iterator& _other) noexcept {
            return _rsrc == _other._rsrc && _offset <= _other._offset;
        }

        [[nodiscard]] constexpr inline bool __stdcall operator>(const random_access_iterator& _other) noexcept {
            return _rsrc == _other._rsrc && _offset > _other._offset;
        }

        [[nodiscard]] constexpr inline bool __stdcall operator>=(const random_access_iterator& _other) noexcept {
            return _rsrc == _other._rsrc && _offset >= _other._offset;
        }
};
