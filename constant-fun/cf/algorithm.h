// cf/algorithm.h                                                     -*-C++-*-
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

#ifndef INCLUDED_CF_ALGORITHM
#define INCLUDED_CF_ALGORITHM

#include "pair.h"
#include <iterator>
#include <utility>

// ----------------------------------------------------------------------------

namespace cf {
    template <typename InIt, typename Difference>
    constexpr InIt prev(InIt it, Difference n, ::std::input_iterator_tag const& category);
    template <typename RndIt, typename Difference>
    constexpr RndIt prev(RndIt it, Difference n, ::std::random_access_iterator_tag const&);
    template <typename InIt>
    constexpr InIt prev(InIt it, typename ::std::iterator_traits<InIt>::difference_type n = 1);

    template <typename InIt, typename Difference>
    constexpr InIt next(InIt it, Difference n, ::std::input_iterator_tag const& category);
    template <typename RndIt, typename Difference>
    constexpr RndIt next(RndIt it, Difference n, ::std::random_access_iterator_tag const&);
    template <typename InIt>
    constexpr InIt next(InIt it, typename ::std::iterator_traits<InIt>::difference_type n = 1);

    template <typename FwdIt, typename EndPos, typename Value>
    constexpr FwdIt find(FwdIt it, EndPos end, Value value);
    template <typename FwdIt, typename EndPos, typename Pred>
    constexpr FwdIt find_if(FwdIt it, EndPos end, Pred pred);

    template <typename FwdIt0, typename FwdIt1>
    constexpr ::cf::pair<FwdIt0, FwdIt1>
    mismatch(FwdIt0 it0, FwdIt0 end0, FwdIt1 it1, FwdIt1 end1);
}

// ----------------------------------------------------------------------------

template <typename InIt, typename Difference>
constexpr InIt cf::prev(InIt it, Difference n, ::std::input_iterator_tag const& category) {
    return Difference() == n? it: ::cf::prev(--it, --n, category);
}
template <typename RndIt, typename Difference>
constexpr RndIt cf::prev(RndIt it, Difference n, ::std::random_access_iterator_tag const&) {
    return it - n;
}
template <typename InIt>
constexpr InIt cf::prev(InIt it, typename ::std::iterator_traits<InIt>::difference_type n) {
    return ::cf::prev(it, n, typename ::std::iterator_traits<InIt>::iterator_category());
}

template <typename InIt, typename Difference>
constexpr InIt cf::next(InIt it, Difference n, ::std::input_iterator_tag const& category) {
    return Difference() == n? it: ::cf::next(++it, --n, category);
}
template <typename RndIt, typename Difference>
constexpr RndIt cf::next(RndIt it, Difference n, ::std::random_access_iterator_tag const&) {
    return it + n;
}
template <typename InIt>
constexpr InIt cf::next(InIt it, typename ::std::iterator_traits<InIt>::difference_type n) {
    return ::cf::next(it, n, typename ::std::iterator_traits<InIt>::iterator_category());
}

// ----------------------------------------------------------------------------

template <typename FwdIt, typename EndPos, typename Value>
constexpr FwdIt cf::find(FwdIt it, EndPos end, Value value) {
    return it == end || *it == value
        ? it
        : ::cf::find(::cf::next(it), end, value);
}

template <typename FwdIt, typename EndPos, typename Pred>
constexpr FwdIt cf::find_if(FwdIt it, EndPos end, Pred pred) {
    return it == end || pred(*it)
        ? it
        : ::cf::find_if(::cf::next(it), end, pred);
}

// ----------------------------------------------------------------------------

template <typename FwdIt0, typename FwdIt1>
constexpr ::cf::pair<FwdIt0, FwdIt1>
cf::mismatch(FwdIt0 it0, FwdIt0 end0, FwdIt1 it1, FwdIt1 end1) {
    return it0 == end0 || it1 == end1 || *it0 != *it1
        ? ::cf::make_pair(it0, it1)
        : ::cf::mismatch(it0 + 1, end0, it1 + 1, end1);
}

// ----------------------------------------------------------------------------


#endif
