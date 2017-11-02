// lomuto_partition.hpp                                               -*-C++-*-
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

#ifndef INCLUDED_LOMUTO_PARTITION
#define INCLUDED_LOMUTO_PARTITION

#include "not_fn.hpp"
#include <algorithm>
#include <functional>

// ----------------------------------------------------------------------------

template <typename FwdIt, typename Predicate>
FwdIt lomuto_partition1(FwdIt it, FwdIt end, Predicate predicate) {
    FwdIt to(it);
    for (; it != end; ++it) {
        if (predicate(*it))
            std::iter_swap(to++, it);
    }
    return to;
}

// ----------------------------------------------------------------------------

template <typename FwdIt, typename Predicate>
FwdIt lomuto_partition2(FwdIt it, FwdIt end, Predicate predicate) {
    FwdIt to(it = std::find_if(it, end, nstd::not_fn(predicate)));
    for (; it != end; ++it) {
        if (predicate(*it))
            std::iter_swap(to++, it);
    }
    return to;
}

// ----------------------------------------------------------------------------

template <typename FwdIt, typename Predicate>
FwdIt lomuto_partition3(FwdIt it, FwdIt end, Predicate predicate) {
    while (it != end && predicate(*it)) {
        ++it;
    }
    FwdIt point = it;
    if (it != end) {
        while (++it != end) {
            if (predicate(*it)) {
                using std::swap;
                swap(*point, *it);
                ++point;
            }
        }
    }
    return point;
}

// ----------------------------------------------------------------------------

#endif
