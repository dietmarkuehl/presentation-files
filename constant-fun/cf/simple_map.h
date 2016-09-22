// cf/simple_map.h                                                    -*-C++-*-
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

#ifndef INCLUDED_CF_SIMPLE_MAP
#define INCLUDED_CF_SIMPLE_MAP

#include "algorithm.h"
#include "functional.h"
#include "pair.h"
#include <stdexcept>
#include <cstddef>

// ----------------------------------------------------------------------------

namespace cf {
    template < ::std::size_t N, typename K, typename V>
    struct simple_map
    {
        using value_type     = ::cf::pair<K const, V>;
        using iterator       = value_type const*;
        using const_iterator = iterator;
        
        value_type elements[N];

        constexpr V access(const_iterator it) const {
            return it == this->end()
                ? throw ::std::runtime_error("no such key")
                : it->second;
        }
        constexpr V operator[](K const& key) const {
            return this->access(this->find(key));
        }
        constexpr const_iterator begin() const { return this->elements; }
        constexpr const_iterator end() const { return this->elements + N; }
        constexpr const_iterator find(K const& key) const {
            return ::cf::find_if(this->begin(), this->end(), ::cf::match1st(key));
        }
    };
}

// ----------------------------------------------------------------------------

#endif
