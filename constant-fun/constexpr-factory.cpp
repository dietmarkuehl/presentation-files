// constexpr-factory.cpp                                              -*-C++-*-
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

#include "cf/simple_map.h"
#include "cf/string_view.h"
#include <iostream>
#include <exception>

// ----------------------------------------------------------------------------

struct base { virtual ~base() {} };
struct foo: base { ~foo() { std::cout << "foo::~foo()\n"; } };
struct bar: base { ~bar() { std::cout << "bar::~bar()\n"; } };
struct faz: base { ~faz() { std::cout << "faz::~faz()\n"; } };

template <typename T> base* make() { return new T(); }

// ----------------------------------------------------------------------------

constexpr cf::simple_map<3, cf::string_view, base*(*)()> factories = {{
    { "foo", make<foo> },
    { "bar", make<bar> },
    { "faz", make<faz> }
}};

int main(int ac, char* av[])
{
    try {
        delete factories[ac == 1? "foo": av[1]]();
    }
    catch (std::exception const& ex) {
        std::cout << "ERROR: " << ex.what() << "\n";
    }
}
