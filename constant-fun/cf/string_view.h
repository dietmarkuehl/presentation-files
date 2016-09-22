// cf/string_view.h                                                   -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2016 Dietmar Kuehl http://www.dietmar-kuehl.de         
//                                                                       
//  Permission is hereby granted, free of charge, to any person          
//  obtaining a copy of this software and associated documentation       
//  files (the "Software"), to deal in the Software without restriction, 
//  including without limitation the rights to use, copy, modify,        
//  merge, publish, distribute, sublicense, and/or sell copies of        
//  the Software, and to permit persons to whom the Software is          
//  furnished to do so, subject to the following conditions:             
//                                                                       
//  The above copyright notice and this permission notice shall be       
//  included in all copies or substantial portions of the Software.      
//                                                                       
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES      
//  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND             
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT          
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,         
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING         
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        
//  OTHER DEALINGS IN THE SOFTWARE. 
// ----------------------------------------------------------------------------

#ifndef INCLUDED_CF_STRING
#define INCLUDED_CF_STRING

#include "algorithm.h"
#include "functional.h"
#include <iterator>
#include <ostream>
#include <cstddef>

// ----------------------------------------------------------------------------

namespace cf
{
    class unreachable {
        template <typename InIt>
        friend constexpr bool operator==(InIt, unreachable) { return false; }
        template <typename InIt>
        friend constexpr bool operator==(unreachable, InIt) { return false; }
        template <typename InIt>
        friend constexpr bool operator!=(InIt, unreachable) { return true; }
        template <typename InIt>
        friend constexpr bool operator!=(unreachable, InIt) { return true; }
    };
    class string_view;
    inline namespace udl {
        inline namespace string_view_udl {
            constexpr ::cf::string_view operator"" _sv(char const *s, ::std::size_t);
        }
    }
}

// ----------------------------------------------------------------------------

class cf::string_view
{
private:
    char const* d_begin, * d_end;

    static constexpr bool compare_equal(::cf::pair<char const*, char const*> it,
                                        ::cf::pair<char const*, char const*> end) {
        return it.first == end.first && it.second == end.second;
    }
    static constexpr bool compare_less(::cf::pair<char const*, char const*> it,
                                       ::cf::pair<char const*, char const*> end) {
        return (it.first != end.first && it.second != end.second)
            ? (*it.first < *it.second)
            : (it.first == end.first && it.second != end.second);
    }
public:
    using value_type             = char;
    using size_type              = ::std::size_t;
    using difference_tpye        = ::std::ptrdiff_t;
    using reference              = char const&;
    using const_reference        = char const&;
    using pointer                = char const*;
    using const_poineter         = char const*;
    using iterator               = char const*;
    using const_iterator         = char const*;
    using reverse_iterator       = ::std::reverse_iterator<iterator>;
    using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
    
    static constexpr size_type npos = -1;
    
    constexpr string_view() noexcept
        : d_begin(""), d_end(this->d_begin) {
    }
    constexpr string_view(string_view const& str, size_type pos, size_type n = npos)
        : d_begin(str.d_begin + pos)
        , d_end(n != npos? this->d_begin + n: str.d_end) {
    }
    constexpr string_view(char const* str, size_type n)
        : d_begin(str)
        , d_end(str + n) {
    }
    constexpr string_view(char const* str) noexcept
        : d_begin(str)
        , d_end(::cf::find(str, ::cf::unreachable(), '\0')) {
    }
    constexpr string_view(char const* begin, char const* end) noexcept
        : d_begin(begin)
        , d_end(end) {
    }
    ~string_view() = default;

    constexpr bool empty() const noexcept {
        return this->d_begin == this->d_end;
    }
    constexpr size_type size() const noexcept {
        return this->d_end - this->d_begin;
    }
    constexpr char const* begin() const noexcept { return this->d_begin; }
    constexpr char const* end()   const noexcept { return this->d_end; }

    constexpr bool operator== (string_view const& other) const noexcept {
        return compare_equal(::cf::mismatch(this->begin(), this->end(),
                                            other.begin(), other.end()),
                             ::cf::make_pair(this->end(), other.end()));
    }
    constexpr bool operator< (string_view const& other) const noexcept {
        return compare_less(::cf::mismatch(this->begin(), this->end(),
                                           other.begin(), other.end()),
                            ::cf::make_pair(this->end(), other.end()));
    }
    constexpr bool operator!= (string_view const& other) const noexcept {
        return !(*this == other);
    }
    constexpr bool operator<= (string_view const& other) const noexcept {
        return !(other < *this);
    }
    constexpr bool operator>  (string_view const& other) const noexcept {
        return other < *this;
    }
    constexpr bool operator>= (string_view const& other) const noexcept {
        return !(*this < other);
    }

    friend ::std::ostream& operator<< (::std::ostream& out, string_view const& s) {
        return out.write(s.d_begin, s.size());
    }
};

// ----------------------------------------------------------------------------

constexpr cf::string_view cf::udl::operator"" _sv(char const *s, ::std::size_t) {
    return cf::string_view(s);
}

// ----------------------------------------------------------------------------

#endif
