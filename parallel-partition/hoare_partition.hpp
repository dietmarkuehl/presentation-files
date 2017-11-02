// hoare_partition.hpp                                                -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2017 Dietmar Kuehl http://www.dietmar-kuehl.de         
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

#ifndef INCLUDED_HOARE_PARTITION
#define INCLUDED_HOARE_PARTITION

#include "not_fn.hpp"
#include <algorithm>
#include <functional>
#include <iterator>

// ----------------------------------------------------------------------------

template <typename BiDirIt, typename Predicate>
BiDirIt hoare_partition(BiDirIt it, BiDirIt end, Predicate predicate) {
    while (true) {
        while (it != end && predicate(*it)) { ++it; }
        while (it != end && !predicate(*--end)) {}
        if (it == end) { break; }
        std::iter_swap(it, end);
        ++it;
    }
    return it;
}

// ----------------------------------------------------------------------------

template <typename BiDirIt, typename Predicate>
BiDirIt algorithm_partition(BiDirIt it, BiDirIt end, Predicate predicate) {
    while (true) {
        it = std::find_if(it, end, nstd::not_fn(predicate));
        end = std::find_if(std::reverse_iterator<BiDirIt>(end),
                           std::reverse_iterator<BiDirIt>(it),
                           predicate).base();
        if (it != end) {
            --end;
        }
        if (it == end) { break; }
        std::iter_swap(it, end);
        ++it;
    }
    return it;
}

// ----------------------------------------------------------------------------

template <typename RndIt, typename Predicate>
RndIt sentinel_partition(RndIt it, RndIt end, Predicate predicate) {
    while (it != end && predicate(*it)) { ++it; }
    while (it != end && !predicate(*--end)) {}
    while (it < end) {
        std::iter_swap(it, end);
        while (predicate(*++it)) {}
        while (!predicate(*--end)) {}
    }
    return it;
}

// ----------------------------------------------------------------------------

#endif
