// constexpr-array.cpp                                                -*-C++-*-
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

#include "cf/algorithm.h"
#include "cf/functional.h"
#include "cf/string_view.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <utility>
#include <cstddef>

// ----------------------------------------------------------------------------

namespace cf {
    template <typename T, std::size_t N>
    constexpr T* begin(T (&array)[N]) {
        return array;
    }
    template <typename T, std::size_t N>
    constexpr T* end(T (&array)[N]) {
        return array + N;
    }
}

// ----------------------------------------------------------------------------

template <typename P, std::size_t N, typename K>
constexpr P const* map_find(P const (&array)[N], K k) {
    return cf::find_if(cf::begin(array), cf::end(array), cf::match1st(k));
}

template <typename P, std::size_t N>
constexpr typename P::second_type map_access_aux(P const (&array)[N], P const* it) {
    return it == cf::end(array)? throw std::runtime_error("key not found"): it->second;
}

template <typename P, std::size_t N, typename K>
constexpr typename P::second_type map_access(P const (&array)[N], K k) {
    return map_access_aux(array, cf::find_if(cf::begin(array), cf::end(array), cf::match1st(k)));
}

// ----------------------------------------------------------------------------

bool f0(char const* s)
{
    using namespace cf::udl;
    namespace PN = cf;
    
    static constexpr PN::pair<cf::string_view, int> map[] = {
        PN::pair<cf::string_view, int>{"one", 1},
        PN::make_pair("two", 2),
        { "three", 3 }
    };

    constexpr PN::pair<cf::string_view, int> const* it = map_find(map, "two"_sv);
    constexpr int v = map_access(map, "two"_sv);

    switch (map_access(map, cf::string_view(s))) {
    case map_access(map, "one"_sv): std::cout << "one\n"; break;
    case map_access(map, "two"_sv): std::cout << "two\n"; break;
    case map_access(map, "three"_sv): std::cout << "three\n"; break;
    }
    return it->second == v;
}

// ----------------------------------------------------------------------------

int main(int ac, char* av[])
{
    f0(ac == 1? "one": av[1]);
#if 0
    constexpr std::pair<std::string, int> map[] = {
        std::pair<std::string, int>("faz", 1),
        std::make_pair("foo", 2),
        { "bar", 3 }
    };
#endif
}
