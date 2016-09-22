// constexpr-mergesort.cpp                                            -*-C++-*-
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

#include "cf/array.h"
#include "cf/pair.h"
#include <algorithm>
#include <iterator>
#include <iostream>
#include <utility>
#include <cstddef>

// ----------------------------------------------------------------------------

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

#if 1
template <typename T, std::size_t N, typename = std::enable_if_t<!(1u < N)>, typename = void>
constexpr cf::array<T, N> merge_sort(cf::array<T, N> a) {
    return a;
}
#else
template <typename T>
constexpr cf::array<T, 0u> merge_sort(cf::array<T, 0u> a) {
    return a;
}
template <typename T>
constexpr cf::array<T, 1u> merge_sort(cf::array<T, 1u> a) {
    return a;
}
#endif

template <typename T, std::size_t N, typename = std::enable_if_t<1u < N> >
cf::array<T, N> constexpr merge_sort(cf::array<T, N> a) {
    return merge(merge_sort(select(a, make_indices<(N+1) / 2, 0>())),
                 merge_sort(select(a, make_indices<N / 2, (N+1) / 2>())));
}

// ----------------------------------------------------------------------------

int main() {
    constexpr auto a = merge_sort(cf::array<int, 9>{{ 9, 1, 8, 2, 7, 3, 6, 4, 5 }});
    std::copy(a.begin(), a.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << "\n";
}
