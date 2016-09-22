// cf/functional.h                                                    -*-C++-*-
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

#ifndef INCLUDED_CF_FUNCTIONAL
#define INCLUDED_CF_FUNCTIONAL

#include "forward.h"

// ----------------------------------------------------------------------------
// With C++14 and later these are not required. However, for C++11 the function
// call operators are not constexpr.

namespace cf
{
    template <typename = void> struct equal_to;
    template <typename = void> struct not_equal_to;
    template <typename = void> struct greater;
    template <typename = void> struct less;
    template <typename = void> struct greater_equal;
    template <typename = void> struct less_equal;

    template <typename Pred> struct not_t;
    template <typename Pred> constexpr not_t<Pred> not_(Pred pred);
    
    template <typename T> struct matcher1st;
    template <typename T> constexpr matcher1st<T> match1st(T value);
}

// ----------------------------------------------------------------------------

template <typename>
struct cf::equal_to
{
    template <typename S, typename T>
    constexpr bool operator()(S&& s, T&& t) const {
        return ::cf::forward<S>(s) == ::cf::forward<T>(t);
    }
};

// ----------------------------------------------------------------------------

template <typename>
struct cf::not_equal_to
{
    template <typename S, typename T>
    constexpr bool operator()(S&& s, T&& t) const {
        return ::cf::forward<S>(s) != ::cf::forward<T>(t);
    }
};

// ----------------------------------------------------------------------------

template <typename>
struct cf::greater
{
    template <typename S, typename T>
    constexpr bool operator()(S&& s, T&& t) const {
        return ::cf::forward<S>(s) > ::cf::forward<T>(t);
    }
};

// ----------------------------------------------------------------------------

template <typename>
struct cf::less
{
    template <typename S, typename T>
    constexpr bool operator()(S&& s, T&& t) const {
        return ::cf::forward<S>(s) < ::cf::forward<T>(t);
    }
};

// ----------------------------------------------------------------------------

template <typename>
struct cf::greater_equal
{
    template <typename S, typename T>
    constexpr bool operator()(S&& s, T&& t) const {
        return ::cf::forward<S>(s) >= ::cf::forward<T>(t);
    }
};

// ----------------------------------------------------------------------------

template <typename>
struct cf::less_equal
{
    template <typename S, typename T>
    constexpr bool operator()(S&& s, T&& t) const {
        return ::cf::forward<S>(s) <= ::cf::forward<T>(t);
    }
};

// ----------------------------------------------------------------------------

template <typename Pred>
struct cf::not_t {
    Pred d_pred;
    template <typename... T>
    constexpr bool operator()(T&&... value) const {
        return !this->d_pred(::cf::forward<T>(value)...);
    }
};

template <typename Pred>
constexpr ::cf::not_t<Pred> cf::not_(Pred pred) {
    return ::cf::not_t<Pred>{ { pred } };
}

// ----------------------------------------------------------------------------

template <typename T>
struct cf::matcher1st {
    T d_value;
    template <typename P>
    constexpr bool operator()(P const& p) const {
        return this->d_value == p.first;
    }
};
template <typename T>
constexpr ::cf::matcher1st<T>
cf::match1st(T value) {
    return ::cf::matcher1st<T>{ value };
}

// ----------------------------------------------------------------------------

#endif
