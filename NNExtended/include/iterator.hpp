#pragma once
#include <cassert>
#include <iterator>
#include <sal.h>
#include <type_traits>
#include <type_traits>

#include <sal.h>

#pragma region __RANDOM_ACCESS_ITERATOR__

template<typename _Ty, typename = typename std::enable_if<std::is_arithmetic_v<_Ty>, bool>::type>
class random_access_iterator { // unchecked random access iterator
        // if invalid memory access happens, the OS may raise an access violation exception, the iterator won't do anything about this in release mode
        // in debug mode, certain preventative asserts may fail, indicating where things went wrong

    public:
        using value_type        = _Ty; //typename std::remove_reference<typename std::remove_all_extents<_Ty>::type>::type;
        using pointer           = value_type*;
        using const_pointer     = const value_type*;
        using reference         = value_type&;
        using const_reference   = const value_type&;
        using difference_type   = signed long long;   // aka ptrdiff_t
        using size_type         = unsigned long long; // aka size_t
        using iterator_category = std::random_access_iterator_tag;

        // clang-format off
#ifndef __TEST__    // for testing purposes make the data members public!
    protected:
#endif
        // clang-format on

        // using an unqualified pointer here will raise errors with const iterables!
        pointer   _rsrc {};   // pointer to the iterable resource
        size_type _length {}; // number of elements in the iterable
        size_type _offset {}; // current position in the iterable

    public: // NOLINT(readability-redundant-access-specifiers)
        constexpr random_access_iterator() noexcept = default;

        // template<size_type _size>
        // constexpr  explicit  random_access_iterator(_In_ _Ty (&_array)[_size]) noexcept :
        //     _rsrc(_array), _length(_size), _offset() {
        //     assert(_array);
        //     assert(_size);
        // }

        // template<size_type _size>
        // constexpr  explicit  random_access_iterator(_In_ _Ty (&_array)[_size], _In_ const size_type& _pos) noexcept :
        //     _rsrc(_array), _length(_size), _offset(_pos) {
        //     assert(_array);
        //     assert(_size);
        //     assert(_size >= _pos);
        // }

        constexpr random_access_iterator(_In_ _Ty* const _res, _In_ const size_type& _len) noexcept :
            _rsrc(_res), _length(_len), _offset() {
            assert(_res);
            assert(_len);
        }

        constexpr random_access_iterator(_In_ _Ty* const _res, _In_ const size_type& _len, _In_ const size_type& _pos) noexcept :
            _rsrc(_res), _length(_len), _offset(_pos) {
            assert(_res);
            assert(_len);
            assert(_len >= _pos);
        }

        constexpr random_access_iterator(_In_ const random_access_iterator& other) noexcept :
            _rsrc(other._rsrc), _length(other._length), _offset(other._offset) { }

        constexpr random_access_iterator(_In_ random_access_iterator&& other) noexcept :
            _rsrc(other._rsrc), _length(other._length), _offset(other._offset) {
            // cleanup the stolen from resource
            other._rsrc   = nullptr;
            other._length = other._offset = 0;
        }

        constexpr random_access_iterator& operator=(_In_ const random_access_iterator& other) noexcept {
            if (this == &other) return *this;
            _rsrc   = other._rsrc;
            _length = other._length;
            _offset = other._offset;
            return *this;
        }

        constexpr random_access_iterator& operator=(_In_ random_access_iterator&& other) noexcept {
            if (this == &other) return *this;
            _rsrc         = other._rsrc;
            _length       = other._length;
            _offset       = other._offset;

            // cleanup
            other._rsrc   = nullptr;
            other._length = other._offset = 0;
            return *this;
        }

        ~random_access_iterator() noexcept {
            _rsrc   = nullptr;
            _length = _offset = 0;
        }

        constexpr reference __stdcall operator*() noexcept { return _rsrc[_offset]; }

        constexpr const_reference __stdcall operator*() const noexcept { return _rsrc[_offset]; }

        constexpr pointer _unwrapped() noexcept { return _rsrc; }

        constexpr const_pointer _unwrapped() const noexcept { return _rsrc; }

        constexpr void reset() noexcept { _offset = 0; }

        constexpr random_access_iterator& __stdcall operator++() noexcept {
            _offset++;
            assert(_offset <= _length);
            return *this;
        }

        constexpr random_access_iterator __stdcall operator++(int) noexcept {
            _offset++;
            assert(_offset <= _length);
            return { _rsrc, _length, _offset - 1 };
        }

        constexpr random_access_iterator& __stdcall operator--() noexcept {
            _offset--;
            assert(_offset <= _length); // assert(_offset >= 0); won't help because _offset is unsigned so instead, check for wraparounds
            return *this;
        }

        constexpr random_access_iterator __stdcall operator--(int) noexcept {
            _offset--;
            assert(_offset <= _length);
            return { _rsrc, _length, _offset + 1 };
        }

        constexpr bool __stdcall operator==(_In_ const random_access_iterator& other) const noexcept {
            return _rsrc == other._rsrc && _offset == other._offset;
        }

        constexpr bool __stdcall operator!=(_In_ const random_access_iterator& other) const noexcept {
            return _rsrc != other._rsrc || _offset != other._offset;
        }

        constexpr bool __stdcall operator<(_In_ const random_access_iterator& other) const noexcept {
            return _rsrc == other._rsrc && _offset < other._offset;
        }

        constexpr bool __stdcall operator<=(_In_ const random_access_iterator& other) const noexcept {
            return _rsrc == other._rsrc && _offset <= other._offset;
        }

        constexpr bool __stdcall operator>(_In_ const random_access_iterator& other) const noexcept {
            return _rsrc == other._rsrc && _offset > other._offset;
        }

        constexpr bool __stdcall operator>=(_In_ const random_access_iterator& other) const noexcept {
            return _rsrc == other._rsrc && _offset >= other._offset;
        }

        template<typename _TyInt>
        constexpr typename std::enable_if<std::is_integral_v<_TyInt>, random_access_iterator>::type operator+(_In_ const _TyInt& stride
        ) const noexcept {
            assert(_length >= _offset + stride);
            return { _rsrc, _length, _offset + stride };
        }

        template<typename _TyInt>
        constexpr typename std::enable_if<std::is_integral<_TyInt>::value, random_access_iterator>::type operator-(_In_ const _TyInt& stride
        ) const noexcept {
            assert(_length >= _offset - stride);
            return { _rsrc, _length, _offset - stride };
        }

        constexpr difference_type operator+(_In_ const random_access_iterator& other) const noexcept {
            assert(_rsrc == other._rsrc && _length == other._length);
            assert(_offset + other._offset <= _length);
            return _offset + other._offset;
        }

        constexpr difference_type operator-(_In_ const random_access_iterator& other) const noexcept {
            assert(_rsrc == other._rsrc && _length == other._length);
            assert(_offset + other._offset <= _length);
            return _offset - other._offset;
        }
};

#pragma endregion

#pragma region __STRIDED_RANDOM_ACCESS_ITERATOR__

template<typename _Ty> class strided_random_access_iterator final :
    public random_access_iterator<_Ty> { // an iterator to capture matrix column elements
        // random_access_iterator cannot be used here as it uses a non modifiable default stride of 1
        // since our matrix class is row major, the vanilla random_access_iterator is unsuitable to iterate over elements of a given column
        // as it would require a custom stride (number of columns) to get to the next element instead of 1!
        // iterating over rows can be accomplished with an aptly customized plain random_access_iterator

        // arithmetic semantics of strided_random_access_iterator are very different from the base random_access_iterator!

    public:
        using value_type        = typename random_access_iterator<_Ty>::value_type;
        using pointer           = typename random_access_iterator<_Ty>::pointer;
        using const_pointer     = typename random_access_iterator<_Ty>::const_pointer;
        using reference         = typename random_access_iterator<_Ty>::reference;
        using const_reference   = typename random_access_iterator<_Ty>::const_reference;
        using difference_type   = typename random_access_iterator<_Ty>::difference_type;
        using size_type         = typename random_access_iterator<_Ty>::size_type;
        using iterator_category = typename random_access_iterator<_Ty>::iterator_category;

        // clang-format off
#ifndef __TEST__
    private:
#endif
        // clang-format on

        // using the injected members from the base class
        using strided_random_access_iterator::random_access_iterator::_length;
        using strided_random_access_iterator::random_access_iterator::_offset;
        using strided_random_access_iterator::random_access_iterator::_rsrc;
        size_type _stride; // stride size, this does not participate in comparison operations between strided_random_access_iterator s

    public: // NOLINT(readability-redundant-access-specifiers)
        constexpr strided_random_access_iterator() noexcept : strided_random_access_iterator::random_access_iterator(), _stride() { }

        constexpr strided_random_access_iterator(_In_ _Ty* const _res, _In_ const size_t& _sz, _In_ const size_t& _str) noexcept :
            strided_random_access_iterator::random_access_iterator { _res, _sz }, _stride { _str } {
            assert(_res);
            assert(_sz);
            assert(_str);
        }

        constexpr strided_random_access_iterator(
            _In_ _Ty* const _res, _In_ const size_t& _sz, _In_ const size_t& _pos, _In_ const size_t& _str
        ) noexcept :
            strided_random_access_iterator::random_access_iterator { _res, _sz, _pos }, _stride { _str } {
            assert(_res);
            assert(_sz);
            assert(_sz >= _pos);
            assert(_str);
        }

        constexpr strided_random_access_iterator(_In_ const strided_random_access_iterator& other) noexcept :
            strided_random_access_iterator::random_access_iterator { other._rsrc, other._length, other._offset },
            _stride { other._stride } { }

        constexpr strided_random_access_iterator(_In_ strided_random_access_iterator&& other) noexcept :
            strided_random_access_iterator::random_access_iterator { other._rsrc, other._length, other._offset },
            _stride { other._stride } {
            other._rsrc   = nullptr;
            other._length = other._offset = other._stride = 0;
        }

        constexpr strided_random_access_iterator& operator=(_In_ const strided_random_access_iterator& other) noexcept {
            if (this == &other) return *this;
            _rsrc   = other._rsrc;
            _length = other._length;
            _offset = other._offset;
            _stride = other._stride;
            return *this;
        }

        constexpr strided_random_access_iterator& operator=(_In_ strided_random_access_iterator&& other) noexcept {
            if (this == &other) return *this;
            _rsrc         = other._rsrc;
            _length       = other._length;
            _offset       = other._offset;
            _stride       = other._stride;

            other._rsrc   = nullptr;
            other._length = other._offset = other._stride = 0;
            return *this;
        }

        ~strided_random_access_iterator() noexcept {
            _rsrc   = nullptr;
            _length = _offset = _stride = 0;
        }

        // for strided_random_access_iterator, we need the increment and decrement operations to be checked because otherwise
        // we could easily jump out of our access zone and we do not want that!

        constexpr strided_random_access_iterator& __stdcall operator++() noexcept {
            if (_offset + _stride <= _length) _offset += _stride; // otherwise do not do anything
            return *this;
        }

        constexpr strided_random_access_iterator __stdcall operator++(int) noexcept {
            if (_offset + _stride <= _length) {
                _offset += _stride;
                return { _rsrc, _length, _offset - _stride };
            }
            return *this;
        }

        constexpr strided_random_access_iterator& __stdcall operator--() noexcept {
            _offset -= _stride;
            assert(_offset <= _length); // assert(_offset >= 0); won't help because _offset is unsigned, so instead check for wraparounds
            return *this;
        }

        constexpr strided_random_access_iterator __stdcall operator--(int) noexcept {
            _offset -= _stride;
            assert(_offset <= _length);
            return { _rsrc, _length, _offset + _stride };
        }

        bool operator==(const strided_random_access_iterator& other) {
            //
        }

        bool operator!=(const strided_random_access_iterator& other) {
            //
        }

        template<typename _TyInt>
        constexpr typename std::enable_if<std::is_integral<_TyInt>::value, strided_random_access_iterator>::type operator+(
            _In_ const _TyInt& distance
        ) const noexcept {
            assert(_length >= _offset + distance);
            return { _rsrc, _length, _offset + distance, _stride };
        }

        template<typename _TyInt>
        constexpr typename std::enable_if<std::is_integral<_TyInt>::value, strided_random_access_iterator>::type operator-(
            _In_ const _TyInt& distance
        ) const noexcept {
            assert(_length >= _offset - distance);
            return { _rsrc, _length, _offset - distance, _stride };
        }

        constexpr difference_type operator+(_In_ const strided_random_access_iterator& other) const noexcept {
            assert(_rsrc == other._rsrc && _length == other._length && _stride == other._stride); // make sure the strides are identical too
            assert(_offset + other._offset <= _length);
            return _offset + other._offset;
        }

        constexpr difference_type operator-(_In_ const strided_random_access_iterator& other) const noexcept {
            assert(_rsrc == other._rsrc && _length == other._length && _stride == other._stride);
            assert(_offset + other._offset <= _length);
            return _offset - other._offset;
        }
};

#pragma endregion

// NOLINTEND(readability-redundant--specifier)
