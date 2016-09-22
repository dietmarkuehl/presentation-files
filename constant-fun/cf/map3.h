// cf/map3.h                                                          -*-C++-*-
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

#ifndef INCLUDED_CF_MAP3
#define INCLUDED_CF_MAP3

// ----------------------------------------------------------------------------

#include "algorithm.h"
#include "array.h"
#include "functional.h"
#include "pair.h"
#include "integer_sequence.h"
#include <iterator>
#include <ostream>
#include <utility>
#include <cstddef>

// ----------------------------------------------------------------------------

namespace cf
{
    template <typename T>
    constexpr std::remove_reference_t<T>&& move(T&& ref) noexcept(true) {
        return static_cast<std::remove_reference_t<T>&&>(ref);
    }

    template <typename RndIt>
    constexpr auto distance(RndIt begin, RndIt end) {
        return end - begin;
    }
    template <typename RndIt, typename Value>
    constexpr RndIt partition(RndIt begin, RndIt end, Value const& pivot) {
        while (true) {
            while (begin != end && *begin < pivot)
                ++begin;
            while (begin != end && !(*--end < pivot));
            if (begin == end) break;
            swap(*begin, *end);
            ++begin;
        }
        return begin;
    }

    template <typename RndIt>
    constexpr void sort(RndIt begin, RndIt end) {
        if (distance(begin, end) <= 1) return;
        RndIt pivot = end - 1;
        RndIt mid = partition(begin, pivot, *pivot);
        swap(*mid, *pivot);
        sort(begin, mid);
        sort(mid + 1, end);
    }

    template <typename Range>
    constexpr Range sort(Range&& range) {
        sort(range.begin(), range.end());
        return range;
    }

    using std::common_type_t;
    enum none {};
    auto constexpr sort_values() { return cf::array<none, 0>(); }
    template <typename... T>
    auto constexpr sort_values(T&&... value) {
        using type = common_type_t<T...>;
        array<type, sizeof...(T)> array{{ value... }};
        sort(array.begin(), array.end());
        return array;
    }
}

// ----------------------------------------------------------------------------

namespace cf {
    template <::std::size_t N, typename P, typename Comp = ::cf::less<>>
    struct map3 {
        using value_type              = std::decay_t<P>;
        using key_type                = typename value_type::first_type;
        using mapped_type             = typename value_type::second_type;
        using key_compare             = Comp;
        using reference               = value_type const&;
        using const_reference         = value_type const&;
        using iterator                = value_type const*;
        using const_iterator          = value_type const*;
        using size_type               = ::std::size_t;
        using difference_type         = ::std::ptrdiff_t;
        using pointer                 = value_type const*;
        using const_pointer           = value_type const*;
        using reverse_iterator        = ::std::reverse_iterator<iterator>;
        using const_reverse_iterator  = ::std::reverse_iterator<const_iterator>;

        static constexpr ::std::size_t static_size = N;

        Comp                     d_comp;
        cf::array<value_type, N> d_values;

        constexpr bool      empty() const { return false; }
        constexpr size_type size() const  { return N; }
        constexpr iterator  begin() const { return this->d_values.begin(); }
        constexpr iterator  end() const   { return this->d_values.end(); }

        template <typename Key>
        constexpr iterator find(Key key) const {
            return ::cf::find_if(this->begin(), this->end(), ::cf::match1st(key));
        }
        template <typename Key>
        constexpr auto operator[](Key key) const -> decltype(this->find(key)->second) {
            return this->find(key)->second;
        }
    };

    template <typename P, typename Comp>
    struct map3<0u, P, Comp> {
        static constexpr ::std::size_t static_size = 0u;

        Comp d_comp;
        template <typename Key, typename Value>
        constexpr map3<1u, ::cf::pair<Key, Value>, Comp> insert(Key key, Value value) const {
            return map3<1u, ::cf::pair<Key, Value>, Comp>{ { this->d_comp }, { ::cf::make_pair(key, value) } };
        }

        constexpr int const* begin() const { return nullptr; }
        constexpr int const* end() const   { return nullptr; }
    };

    template <typename... F, typename... S>
    constexpr auto make_map3(::cf::pair<F, S>... values) {
        using CF = std::common_type_t<F...>;
        using CS = std::common_type_t<S...>;
        return cf::map3<sizeof...(F), cf::pair<CF, CS>>{ { cf::less<>{} },
                sort_values(values...)
            };
    }

    template <std::size_t N, typename P, typename Comp>
    ::std::ostream& operator<< (::std::ostream& out,
                                map3<N, P, Comp> const& map) {
        for (auto&& value: map) {
            out << value << "\n";
        }
        return out;
    }
}

// ----------------------------------------------------------------------------

#endif
