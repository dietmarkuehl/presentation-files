// constexpr-map.cpp                                                  -*-C++-*-
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

#include "cf/map1.h"
#include "cf/string_view.h"
#include <iostream>

// ----------------------------------------------------------------------------

using namespace cf::udl;

constexpr auto map1 = cf::make_map1_zero()
    .insert("zero"_sv,  0)
    .insert("one"_sv,   1)
    .insert("two"_sv,   2)
    .insert("three"_sv, 3)
    .insert("four"_sv,  4)
    .insert("five"_sv,  5)
    .insert("six"_sv,   6)
    .insert("seven"_sv, 7)
    .insert("eight"_sv, 8)
    .insert("nine"_sv,  9)
    ;
constexpr auto map2 = cf::make_map1(cf::make_pair("zero"_sv,  0),
                                    cf::make_pair("one"_sv,   1),
                                    cf::make_pair("two"_sv,   2),
                                    cf::make_pair("three"_sv, 3),
                                    cf::make_pair("four"_sv,  4),
                                    cf::make_pair("five"_sv,  5),
                                    cf::make_pair("six"_sv,   6),
                                    cf::make_pair("seven"_sv, 7),
                                    cf::make_pair("eight"_sv, 8),
                                    cf::make_pair("nine"_sv,  9));

enum class E {
    five = map1["five"_sv],
    // ten  = map1["ten"_sv],
};

int main()
{
    std::cout << "map1=" << map1 << "\n";
    std::cout << "map2=" << map2 << "\n";
    static_assert(map1.find("four"_sv)->second == 4, "map1.find(four)");
    static_assert(map1["four"_sv] == 4, "map1[four]");
}
