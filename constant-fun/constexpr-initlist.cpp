// constexpr-initlist.cpp                                             -*-C++-*-
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

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <iterator>

struct foo {
    std::initializer_list<int> l;
    constexpr foo(std::initializer_list<int> list) : l(list) {}
    int const* begin() const { return l.begin(); }
    int const* end() const { return l.begin(); }
};

std::ostream& operator<< (std::ostream& out, foo const& f) {
    std::copy(f.begin(), f.end(),
              std::ostream_iterator<int>(out, " "));
    return out;
}

constexpr foo g() {
    return foo{ 4, 5, 6, 7 };
}

int main()
{
    constexpr foo f{ 1, 2, 3, 4 };
    std::cout << f << "\n";
    std::cout << g() << "\n";
}
