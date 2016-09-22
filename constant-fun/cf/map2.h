// cf/map2.h                                                          -*-C++-*-
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

#ifndef INCLUDED_CF_MAP2
#define INCLUDED_CF_MAP2

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
    template <std::size_t Start, std::size_t... I>
    constexpr std::index_sequence<(I + Start)...> make_indices_aux(std::index_sequence<I...>) {
        return std::index_sequence<(I + Start)...>();
    }

    template <std::size_t Size, std::size_t Start = 0u>
    constexpr decltype(make_indices_aux<Start>(std::make_index_sequence<Size>()))
        make_indices()
    {
        return make_indices_aux<Start>(std::make_index_sequence<Size>());
    }

// ----------------------------------------------------------------------------

    template <typename T>
    constexpr void use(T) {
    }

    template <typename T, std::size_t N, std::size_t... I>
    constexpr cf::array<T, sizeof...(I)> select(cf::array<T, N> a, std::index_sequence<I...>) {
        use(a);
        return cf::array<T, sizeof...(I)>{ a[I]... };
    }

    template <typename T, std::size_t N>
    constexpr cf::array<T, N - 1u> tail(cf::array<T, N> a) {
        return select(a, make_indices<N - 1u, 1u>());
    }

// ----------------------------------------------------------------------------

    template <typename T, std::size_t N, std::size_t...I>
    constexpr cf::array<T, N + 1u> cons_aux(T v, cf::array<T, N> a, std::index_sequence<I...>) {
        return cf::array<T, N + 1>{{ v, a[I]... }};
    }

    template <typename T, std::size_t N>
    constexpr cf::array<T, N + 1u> cons(T v, cf::array<T, N> a) {
        return cons_aux(v, a, make_indices<N>());
    }

// ----------------------------------------------------------------------------

    template <typename T, std::size_t N>
    constexpr cf::array<T, N> merge(cf::array<T, N> a, cf::array<T, 0>) {
        return a;
    }

    template <typename T, std::size_t N>
    constexpr cf::array<T, N> merge(cf::array<T, 0>, cf::array<T, N> a) {
        return a;
    }

    template <typename T, std::size_t N1, std::size_t N2>
    constexpr cf::array<T, N1 + N2> merge(cf::array<T, N1> a1, cf::array<T, N2> a2) {
        return a1[0] < a2[0]
                       ? cons(a1[0], merge(tail(a1), a2))
                       : cons(a2[0], merge(a1, tail(a2)));
    }
 
// ----------------------------------------------------------------------------

    template <typename T, std::size_t N, typename = std::enable_if_t<!(1u < N)>, typename = void>
    constexpr cf::array<T, N> merge_sort(cf::array<T, N> a) {
        return a;
    }

    template <typename T, std::size_t N, typename = std::enable_if_t<1u < N> >
              cf::array<T, N> constexpr merge_sort(cf::array<T, N> a) {
        return merge(merge_sort(select(a, make_indices<(N+1) / 2, 0>())),
                     merge_sort(select(a, make_indices<N / 2, (N+1) / 2>())));
    }
}


namespace cf {
    template <::std::size_t N, typename P, typename Comp = ::cf::less<>>
    struct map2 {
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
    struct map2<0u, P, Comp> {
        static constexpr ::std::size_t static_size = 0u;

        Comp d_comp;
        template <typename Key, typename Value>
        constexpr map2<1u, ::cf::pair<Key, Value>, Comp> insert(Key key, Value value) const {
            return map2<1u, ::cf::pair<Key, Value>, Comp>{ { this->d_comp }, { ::cf::make_pair(key, value) } };
        }

        constexpr int const* begin() const { return nullptr; }
        constexpr int const* end() const   { return nullptr; }
    };

    template <typename... F, typename... S>
    constexpr auto make_map2(::cf::pair<F, S>... values) {
        using CF = std::common_type_t<F...>;
        using CS = std::common_type_t<S...>;
        return cf::map2<sizeof...(F), cf::pair<CF, CS>>{ { cf::less<>{} },
                merge_sort(cf::array<cf::pair<CF, CS>, sizeof...(F)>{{ values... }})
            };
    }


    template <std::size_t N, typename P, typename Comp>
    ::std::ostream& operator<< (::std::ostream& out,
                                map2<N, P, Comp> const& map) {
        for (auto&& value: map) {
            out << value << "\n";
        }
        return out;
    }
}

// ----------------------------------------------------------------------------

#endif
