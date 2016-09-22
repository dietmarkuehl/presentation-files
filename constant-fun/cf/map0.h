// cf/map0.h                                                           -*-C++-*-
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

#ifndef INCLUDED_CF_MAP0
#define INCLUDED_CF_MAP0

#include "algorithm.h"
#include "functional.h"
#include "pair.h"
#include "integer_sequence.h"
#include <iterator>
#include <ostream>
#include <type_traits>
#include <cstddef>

// ----------------------------------------------------------------------------

namespace cf {
    template <::std::size_t N, typename P, typename Comp = ::cf::less<>>
    struct map0 {
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

        Comp          d_comp;
        value_type    d_values[N];

        constexpr bool      empty() const { return false; }
        constexpr size_type size() const  { return N; }
        constexpr iterator  begin() const { return this->d_values; }
        constexpr iterator  end() const   { return this->d_values + N; }

        template <typename Key>
        constexpr iterator find(Key key) const {
            return ::cf::find_if(this->begin(), this->end(), ::cf::match1st(key));
        }
        template <typename Key>
        constexpr auto operator[](Key key) const -> decltype(this->find(key)->second) {
            return this->find(key)->second;
        }

        template <::std::size_t... I>
        constexpr map0<N + 1u, P, Comp> build(P p, ::cf::index_sequence<I...>) const {
            return map0<N + 1u, P, Comp>{ { this->d_comp}, { this->d_values[I]..., p } };
        }

        constexpr map0<N + 1u, P, Comp> insert(P pair) const {
            return this->build(pair, ::cf::make_index_sequence<N>());
        }
        template <typename Key, typename Value>
        constexpr map0<N + 1u, P, Comp> insert(Key key, Value value) const {
            return this->build(::cf::make_pair(key, value), ::cf::make_index_sequence<N>());
        }
    };

    template <typename P, typename Comp>
    struct map0<0u, P, Comp> {
        static constexpr ::std::size_t static_size = 0u;

        Comp d_comp;
        template <typename Key, typename Value>
        constexpr map0<1u, ::cf::pair<Key, Value>, Comp> insert(Key key, Value value) const {
            return map0<1u, ::cf::pair<Key, Value>, Comp>{ { this->d_comp }, { ::cf::make_pair(key, value) } };
        }
        template <typename Pair>
        constexpr map0<1u, Pair, Comp> insert(Pair pair) const {
            return map0<1u, Pair, Comp>{ { this->d_comp }, { pair } };
        }


        constexpr int const* begin() const { return nullptr; }
        constexpr int const* end() const   { return nullptr; }
    };

    template <typename Comp = ::cf::less<> >
    constexpr map0<0u, void, Comp> make_map0_zero(Comp comp = Comp()) {
        return map0<0u, void, Comp>{comp};
    }

    template <typename Map>
    constexpr Map build_map0(Map map) {
        return map;
    }

    template <typename Map, typename Value, typename... T>
    constexpr map0<Map::static_size + sizeof...(T) + 1u, std::decay_t<Value>, ::cf::less<>>
        build_map0(Map map, Value&& value, T&&... values) {
        return build_map0(map.insert(value), ::cf::forward<T>(values)...);
    }

    template <typename... T>
    constexpr auto make_map0(T&&... values)
        -> decltype(::cf::build_map0(map0<0u, void, ::cf::less<>>{::cf::less<>()}, ::cf::forward<T>(values)...)) {
        return ::cf::build_map0(map0<0u, void, ::cf::less<>>{::cf::less<>()}, ::cf::forward<T>(values)...);
    }


    template <std::size_t N, typename P, typename Comp>
    ::std::ostream& operator<< (::std::ostream& out,
                                map0<N, P, Comp> const& map) {
        for (auto&& value: map) {
            out << value << "\n";
        }
        return out;
    }
}

// ----------------------------------------------------------------------------

#endif
