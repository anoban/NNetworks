#pragma once
#ifndef __ITERATOR_HPP__
    #define __ITERATOR_HPP__
    #include <cassert>
    #include <iterator>

// a custom unchecked random access iterator to be used with idx classes and matrix classes
// designed to use with arithmetic scalar types
template<typename T> requires std::is_arithmetic_v<T> class random_access_iterator final {
    public:
        using value_type        = typename std::remove_cv_t<T>;
        using pointer           = value_type*;
        using const_pointer     = const value_type*;
        using reference         = value_type&;
        using const_reference   = const value_type&;
        using size_type         = unsigned long long;
        using difference_type   = ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;

    private:
        pointer   _rsrc;
        size_type _length;
        size_type _offset;

    public:
        constexpr inline __cdecl random_access_iterator() noexcept : _rsrc {}, _length {}, _offset {} { }

        constexpr inline __cdecl random_access_iterator(const_pointer _ptr, const size_type& _len) noexcept :
            _rsrc { _ptr }, _length { _len }, _offset {} { }

        constexpr inline __cdecl random_access_iterator(const_pointer _ptr, const size_type& _len, const size_type& _pos) noexcept :
            _rsrc { _ptr }, _length { _len }, _offset { _pos } {
            assert(_offset <= _length); // equality in the conditional is needed when end() calls hand over one-off the end pointers
        }

        constexpr inline __cdecl random_access_iterator(const random_access_iterator& other) noexcept :
            _rsrc { other._rsrc }, _length { other._length }, _offset { other._offset } { }

        constexpr inline __cdecl random_access_iterator(random_access_iterator&& other) noexcept :
            _rsrc { other._rsrc }, _length { other._length }, _offset { other._offset } {
            other._rsrc   = nullptr;
            other._length = other._offset = 0;
        }

        constexpr inline random_access_iterator& __cdecl operator=(const random_access_iterator& other) noexcept {
            if (this == &other) return *this;
            _rsrc   = other._rsrc;
            _length = other._length;
            _offset = other._offset;
            return *this;
        }

        constexpr inline random_access_iterator& __cdecl operator=(random_access_iterator&& other) noexcept {
            if (this == &other) return *this;
            _rsrc         = other._rsrc;
            _length       = other._length;
            _offset       = other._offset;

            other._rsrc   = nullptr;
            other._length = other._offset = 0;
            return *this;
        }

        constexpr inline __cdecl ~random_access_iterator() noexcept {
            _rsrc   = nullptr;
            _length = _offset = 0;
        }

        constexpr inline random_access_iterator& __stdcall operator++() noexcept {
            _offset++;
            assert(_offset <= _length);
            return *this;
        }

        constexpr inline random_access_iterator __stdcall operator++(int) noexcept {
            _offset++;
            assert(_offset <= _length);
            return { _rsrc, _length, _offset - 1 };
        }

        constexpr inline random_access_iterator& __stdcall operator--() noexcept {
            _offset--;
            assert(_offset <= _length); // _offset is unsigned so, cannot ever become negative! check for wrap arounds
            return *this;
        }

        constexpr inline random_access_iterator __stdcall operator--(int) noexcept {
            _offset--;
            assert(_offset <= _length);
            return { _rsrc, _length, _offset + 1 };
        }
};

#endif // !__ITERATOR_HPP__
