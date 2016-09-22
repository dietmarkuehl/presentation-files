// cf/integer_sequence.h                                              -*-C++-*-
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

#ifndef INCLUDED_CF_INTEGER_SEQUENCE
#define INCLUDED_CF_INTEGER_SEQUENCE

#include <cstddef>

// ----------------------------------------------------------------------------

namespace cf {
    template <typename T, T...> struct integer_sequence {};
    template <::std::size_t... I>
        using index_sequence = integer_sequence<::std::size_t, I...>;

    namespace detail {
        template <typename T, T Start, T End>
        struct make_integer_sequence {
            template <T Last, T... Tail> struct make;
            template <T... Tail>
            struct make<Start, Tail...> {
                using type = ::cf::integer_sequence<T, Start, Tail...>;
            };
            template <T Last, T... Tail>
            struct make {
                using type = typename make<Last - 1, Last, Tail...>::type;
            };

            using type = typename make<End -1>::type;
        };
        template <typename T, T StartEnd>
        struct make_integer_sequence<T, StartEnd, StartEnd> {
           using type = ::cf::integer_sequence<T>;
        };
    }

    template <typename T, T N>
        using make_integer_sequence
            = typename ::cf::detail::make_integer_sequence<T, 0, N>::type;
    template <::std::size_t N>
        using make_index_sequence = ::cf::make_integer_sequence<::std::size_t, N>;
}

// ----------------------------------------------------------------------------

#endif
