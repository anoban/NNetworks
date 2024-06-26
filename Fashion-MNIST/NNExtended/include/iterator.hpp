#pragma once
#ifndef __ITERATOR_HPP__
    #define __ITERATOR_HPP__
    #include <cassert>
    #include <compare>
    #include <iterator>
    #include <type_traits>

    #pragma region RANDOM_ACCESS_ITERATOR
template<typename T> class random_access_iterator final { // unchecked random access iterator
        // if invalid memory access happens, the OS may raise an access violation exception, the iterator won't do anything about this in release mode
        // in debug mode, certain preventative asserts may fail, indicating where things went wrong

    public:
        using value_type             = T;
        // T preserves constness of the resource pointer, typename std::remove_cv_t<T> would have removed the qualifiers
        // we would end up with a unqualified pointer even if the passed resource is a const iterable
        using unqualified_value_type = typename std::remove_cv_t<T>;
        using pointer                = T*;
        // T* preserves const correctness if T is a const qualified type, value_type* would have removed that const qualifier
        using const_pointer          = const unqualified_value_type*;
        using reference              = T&; // T& instead of value_type& for the sake of const correctness
        using const_reference        = const unqualified_value_type&;
        using difference_type        = signed long long;   // aka ptrdiff_t
        using size_type              = unsigned long long; // aka size_t
        using iterator_category      = std::random_access_iterator_tag;

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

        constexpr inline __cdecl random_access_iterator(_In_ const random_access_iterator& other) noexcept :
            _rsrc(other._rsrc), _length(other._length), _offset(other._offset) { }

        constexpr inline __cdecl random_access_iterator(_In_ random_access_iterator&& other) noexcept :
            _rsrc(other._rsrc), _length(other._length), _offset(other._offset) {
            // cleanup the stolen from resource
            other._rsrc   = nullptr;
            other._length = other._offset = 0;
        }

        constexpr inline random_access_iterator& __cdecl operator=(_In_ const random_access_iterator& other) noexcept {
            if (this == &other) return *this;
            _rsrc   = other._rsrc;
            _length = other._length;
            _offset = other._offset;
            return *this;
        }

        constexpr inline random_access_iterator& __cdecl operator=(_In_ random_access_iterator&& other) noexcept {
            if (this == &other) return *this;
            _rsrc         = other._rsrc;
            _length       = other._length;
            _offset       = other._offset;

            // cleanup
            other._rsrc   = nullptr;
            other._length = other._offset = 0;
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
            assert(_offset <= _length);
            return { _rsrc, _length, _offset + 1 };
        }

    #ifdef __ITERATOR_USE_STARSHIP_COMPARISON_OPERATOR__ // aka "I'm not worried about performance"

        // defining a custom <=> operator because we want only the _offset member to participate in the comparison
        // we do not care about the _length member and _rsrc being identical is rather a precondition for comparison than a criteria for the pedciation
        [[nodiscard]] constexpr inline std::strong_ordering __stdcall operator<=>(_In_ const random_access_iterator& other) noexcept {
            return _rsrc == other._rsrc && _offset <=> other._offset; // the first subexpression is a prerequisite
            // let the second predicate of the composition dictate the final ordering
        }

    #else
        [[nodiscard]] constexpr inline bool __stdcall operator==(_In_ const random_access_iterator& other) const noexcept {
            return _rsrc == other._rsrc && _offset == other._offset;
        }

        [[nodiscard]] constexpr inline bool __stdcall operator!=(_In_ const random_access_iterator& other) const noexcept {
            return _rsrc != other._rsrc || _offset != other._offset;
        }

        [[nodiscard]] constexpr inline bool __stdcall operator<(_In_ const random_access_iterator& other) const noexcept {
            return _rsrc == other._rsrc && _offset < other._offset;
        }

        [[nodiscard]] constexpr inline bool __stdcall operator<=(_In_ const random_access_iterator& other) const noexcept {
            return _rsrc == other._rsrc && _offset <= other._offset;
        }

        [[nodiscard]] constexpr inline bool __stdcall operator>(_In_ const random_access_iterator& other) const noexcept {
            return _rsrc == other._rsrc && _offset > other._offset;
        }

        [[nodiscard]] constexpr inline bool __stdcall operator>=(_In_ const random_access_iterator& other) const noexcept {
            return _rsrc == other._rsrc && _offset >= other._offset;
        }

    #endif // !__ITERATOR_USE_STARSHIP_COMPARISON_OPERATOR__

        template<typename integral_t> requires std::integral<integral_t>
        [[nodiscard]] constexpr inline random_access_iterator operator+(_In_ const integral_t& stride) const noexcept {
            assert(_length >= _offset + stride);
            return { _rsrc, _length, _offset + stride };
        }

        template<typename integral_t> requires std::integral<integral_t>
        [[nodiscard]] constexpr inline random_access_iterator operator-(_In_ const integral_t& stride) const noexcept {
            assert(_length >= _offset - stride);
            return { _rsrc, _length, _offset - stride };
        }

        [[nodiscard]] constexpr inline difference_type operator+(_In_ const random_access_iterator& other) const noexcept {
            assert(_rsrc == other._rsrc && _length == other._length);
            assert(_offset + other._offset <= _length);
            return _offset + other._offset;
        }

        [[nodiscard]] constexpr inline difference_type operator-(_In_ const random_access_iterator& other) const noexcept {
            assert(_rsrc == other._rsrc && _length == other._length);
            assert(_offset + other._offset <= _length);
            return _offset - other._offset;
        }
};
    #pragma endregion

    #pragma region MATRIX_ITERATOR
template<typename T> class matrix_iterator final { // unchecked random access iterator
        // if invalid memory access happens, the OS may raise an access violation exception, the iterator won't do anything about this in release mode
        // in debug mode, certain preventative asserts may fail, indicating where things went wrong

    public:
        using value_type             = T;
        using unqualified_value_type = typename std::remove_cv_t<T>;
        using pointer                = T*;
        using const_pointer          = const unqualified_value_type*;
        using reference              = T&;
        using const_reference        = const unqualified_value_type&;
        using difference_type        = signed long long;
        using size_type              = unsigned long long;
        using iterator_category      = std::random_access_iterator_tag;

        // clang-format off
#if !defined(_DEBUG) && !defined(__TEST__)   
    private:
#endif
        // clang-format on

        pointer   _rsrc;
        size_type _length;
        size_type _offset;
        size_type _stride; // number of columns or number of rows

    public:
        // will require an explicit template type specification
        constexpr inline __cdecl matrix_iterator() noexcept : _rsrc(), _length(), _offset() { }

        // resource pointer and length of the iterable
        constexpr inline __cdecl matrix_iterator(_In_ T* const _res, _In_ const size_t& _sz) noexcept :
            _rsrc(_res), _length(_sz), _offset() { }

        // resource pointer, length of the iterable and the current iterator position, will need this for .end() methods
        constexpr inline __cdecl matrix_iterator(_In_ T* const _res, _In_ const size_t& _sz, _In_ const size_t& _pos) noexcept :
            _rsrc(_res), _length(_sz), _offset(_pos) {
            assert(_sz >= _pos);
        }

        constexpr inline __cdecl matrix_iterator(_In_ const matrix_iterator& other) noexcept :
            _rsrc(other._rsrc), _length(other._length), _offset(other._offset) { }

        constexpr inline __cdecl matrix_iterator(_In_ matrix_iterator&& other) noexcept :
            _rsrc(other._rsrc), _length(other._length), _offset(other._offset) {
            // cleanup the stolen from resource
            other._rsrc   = nullptr;
            other._length = other._offset = 0;
        }

        constexpr inline matrix_iterator& __cdecl operator=(_In_ const matrix_iterator& other) noexcept {
            if (this == &other) return *this;
            _rsrc   = other._rsrc;
            _length = other._length;
            _offset = other._offset;
            return *this;
        }

        constexpr inline matrix_iterator& __cdecl operator=(_In_ matrix_iterator&& other) noexcept {
            if (this == &other) return *this;
            _rsrc         = other._rsrc;
            _length       = other._length;
            _offset       = other._offset;

            // cleanup
            other._rsrc   = nullptr;
            other._length = other._offset = 0;
            return *this;
        }

        constexpr inline __cdecl ~matrix_iterator() noexcept {
            _rsrc   = nullptr;
            _length = _offset = 0;
        }

        // using the __stdcall calling convention to very frequently invoked methods!

        [[nodiscard]] constexpr inline reference __stdcall operator*() noexcept { return _rsrc[_offset]; }

        [[nodiscard]] constexpr inline const_reference __stdcall operator*() const noexcept { return _rsrc[_offset]; }

        [[nodiscard]] constexpr inline pointer __cdecl _Unwrapped() noexcept { return _rsrc; }

        [[nodiscard]] constexpr inline const_pointer __cdecl _Unwrapped() const noexcept { return _rsrc; }

        constexpr inline matrix_iterator& __stdcall operator++() noexcept {
            _offset++;
            assert(_offset <= _length);
            return *this;
        }

        [[nodiscard]] constexpr inline matrix_iterator __stdcall operator++(int) noexcept {
            _offset++;
            assert(_offset <= _length);
            return { _rsrc, _length, _offset - 1 };
        }

        constexpr inline matrix_iterator& __stdcall operator--() noexcept {
            _offset--;
            assert(_offset <= _length); // assert(_offset >= 0); won't help because _offset is unsigned, so instead check for wraparounds
            return *this;
        }

        [[nodiscard]] constexpr inline matrix_iterator __stdcall operator--(int) noexcept {
            _offset--;
            assert(_offset <= _length);
            return { _rsrc, _length, _offset + 1 };
        }

    #ifdef __ITERATOR_USE_STARSHIP_COMPARISON_OPERATOR__ // aka "I'm not worried about performance"

        // defining a custom <=> operator because we want only the _offset member to participate in the comparison
        // we do not care about the _length member and _rsrc being identical is rather a precondition for comparison than a criteria for the pedciation
        [[nodiscard]] constexpr inline std::strong_ordering __stdcall operator<=>(_In_ const matrix_iterator& other) noexcept {
            return _rsrc == other._rsrc && _offset <=> other._offset; // the first subexpression is a prerequisite
            // let the second predicate of the composition dictate the final ordering
        }

    #else
        [[nodiscard]] constexpr inline bool __stdcall operator==(_In_ const matrix_iterator& other) const noexcept {
            return _rsrc == other._rsrc && _offset == other._offset;
        }

        [[nodiscard]] constexpr inline bool __stdcall operator!=(_In_ const matrix_iterator& other) const noexcept {
            return _rsrc != other._rsrc || _offset != other._offset;
        }

        [[nodiscard]] constexpr inline bool __stdcall operator<(_In_ const matrix_iterator& other) const noexcept {
            return _rsrc == other._rsrc && _offset < other._offset;
        }

        [[nodiscard]] constexpr inline bool __stdcall operator<=(_In_ const matrix_iterator& other) const noexcept {
            return _rsrc == other._rsrc && _offset <= other._offset;
        }

        [[nodiscard]] constexpr inline bool __stdcall operator>(_In_ const matrix_iterator& other) const noexcept {
            return _rsrc == other._rsrc && _offset > other._offset;
        }

        [[nodiscard]] constexpr inline bool __stdcall operator>=(_In_ const matrix_iterator& other) const noexcept {
            return _rsrc == other._rsrc && _offset >= other._offset;
        }

    #endif // !__ITERATOR_USE_STARSHIP_COMPARISON_OPERATOR__

        template<typename integral_t> requires std::integral<integral_t>
        [[nodiscard]] constexpr inline matrix_iterator operator+(_In_ const integral_t& stride) const noexcept {
            assert(_length >= _offset + stride);
            return { _rsrc, _length, _offset + stride };
        }

        template<typename integral_t> requires std::integral<integral_t>
        [[nodiscard]] constexpr inline matrix_iterator operator-(_In_ const integral_t& stride) const noexcept {
            assert(_length >= _offset - stride);
            return { _rsrc, _length, _offset - stride };
        }

        [[nodiscard]] constexpr inline difference_type operator+(_In_ const matrix_iterator& other) const noexcept {
            assert(_rsrc == other._rsrc && _length == other._length);
            assert(_offset + other._offset <= _length);
            return _offset + other._offset;
        }

        [[nodiscard]] constexpr inline difference_type operator-(_In_ const matrix_iterator& other) const noexcept {
            assert(_rsrc == other._rsrc && _length == other._length);
            assert(_offset + other._offset <= _length);
            return _offset - other._offset;
        }
};
    #pragma endregion

#endif // !__ITERATOR_HPP__