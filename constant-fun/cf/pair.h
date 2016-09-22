// cf/pair.h                                                          -*-C++-*-
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

#ifndef INCLUDED_CF_PAIR
#define INCLUDED_CF_PAIR

#include "algorithm.h"
#include "swap.h"
#include "forward.h"
#include <ostream>
#include <type_traits>

// ----------------------------------------------------------------------------
// The definitions in this file are not needed for C++14 and later as all
// relevant functions are required to be constexpr

namespace cf
{
    template <typename First, typename Second>
    struct pair;
    template <typename First, typename Second>
    constexpr pair<typename ::std::decay<First>::type,
                   typename ::std::decay<Second>::type>
    make_pair(First&& first, Second&& second);

    template <typename First, typename Second>
    ::std::ostream& operator<< (::std::ostream& out,
                                ::cf::pair<First, Second> const& pair);

    inline namespace udl
    {
        template <typename First, typename Second>
        constexpr pair<typename ::std::decay<First>::type,
                       typename ::std::decay<Second>::type>
        mp(First&& first, Second&& second);
    }

    template <typename First, typename Second>
    constexpr bool operator< (pair<First, Second> const&, pair<First, Second> const&);
}

// ----------------------------------------------------------------------------

template <typename First, typename Second>
struct cf::pair
{
    using first_type  = First;
    using second_type = Second;
    First  first;
    Second second;

    template <typename F, typename S>
    constexpr operator ::cf::pair<F, S>() const {
        return ::cf::pair<F, S>{ this->first, this->second };
    }

    friend constexpr void swap(pair& a, pair& b) {
        cf::swap(a.first, b.first);
        cf::swap(a.second, b.second);
    }
};

// ----------------------------------------------------------------------------

template <typename First, typename Second>
constexpr cf::pair<typename ::std::decay<First>::type,
                   typename ::std::decay<Second>::type>
cf::make_pair(First&& first, Second&& second) {
    return cf::pair<typename ::std::decay<First>::type,
                    typename ::std::decay<Second>::type>{::cf::forward<First>(first),
                                                         ::cf::forward<Second>(second)};
}

template <typename First, typename Second>
constexpr cf::pair<typename ::std::decay<First>::type,
                   typename ::std::decay<Second>::type>
cf::udl::mp(First&& first, Second&& second) {
    return cf::pair<typename ::std::decay<First>::type,
                    typename ::std::decay<Second>::type>{::cf::forward<First>(first),
                                                         ::cf::forward<Second>(second)};
}

// ----------------------------------------------------------------------------

template <typename First, typename Second>
::std::ostream& cf::operator<< (::std::ostream& out,
                                ::cf::pair<First, Second> const& pair) {
    return out << '(' << pair.first << ", " << pair.second << ')';
}

// ----------------------------------------------------------------------------

template <typename First, typename Second>
constexpr bool cf::operator< (cf::pair<First, Second> const& p1, cf::pair<First, Second> const& p2)
{
    return p1.first < p2.first;
}

// ----------------------------------------------------------------------------

#endif
