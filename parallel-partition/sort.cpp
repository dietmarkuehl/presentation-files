// parallel_sort.t.cpp                                                -*-C++-*-
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

#include "timer.hpp"
#include "thread_pool.hpp"
#include "parallel_sort.hpp"
#include <algorithm>
#include <exception>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>
#include <utility>
#include <vector>

// ----------------------------------------------------------------------------

template <typename Sort, typename Container, typename Compare>
auto test(Sort sort, Container container, Compare compare)
    -> std::pair<utility::timer_duration, bool> {
    utility::timer timer;
    timer.start();
    sort(container.begin(), container.end(), compare);
    auto time = timer.stop();
    bool rc = std::is_sorted(container.begin(), container.end(), compare);
    return { time, rc };
}

// ----------------------------------------------------------------------------

template <typename Partition, typename Container, typename Predicate>
void test(std::string const& name, Partition partition,
          Container const& container, Predicate predicate) {
    auto p = test(partition, container, predicate);
    std::cout << std::setw(60) << name << ' '
              << (p.second? "passed": "\x1b[31mfailed\x1b[0m") << ' '
              << p.first << ' '
              << '\n' << std::flush;
}

// ----------------------------------------------------------------------------

template <typename Compare>
void run_test(std::vector<int> const& v, Compare compare)
{
    nstd::thread_pool pool(128);
    pool.start();

    test("std::sort", [](auto begin, auto end, auto compare) {
            return std::sort(begin, end, compare);
        }, v, compare);
    test("std::stable_sort", [](auto begin, auto end, auto compare) {
            return std::stable_sort(begin, end, compare);
        }, v, compare);
    test("parallel_sort_with_async",
         parallel_sort_with_async<nstd::block_manager_padded_atomic>(pool), v, compare);
}

// ----------------------------------------------------------------------------

void run_tests(int size)
{
    std::minstd_rand rnd(0);
    std::vector<int> v;
    std::cout << "generating\n" << std::flush;
    std::generate_n(std::back_inserter(v), size, [size,&rnd]{ return rnd() % size; });

    std::cout << "--- size=" << size << '\n';
    auto compare([](auto const& v0, auto const& v1){ return v0 < v1; });
    run_test(v, compare);
}

// ----------------------------------------------------------------------------

int main() {
    try {
        for (auto size: {
                1000ull,
                    10000ull,
                    100000ull,
                    1000000ull,
                    10000000ull,
#if 0
                    100000000ull,
                    1000000000ull
#endif
                    }) {
            run_tests(size);
        }
    }
    catch (std::exception const& ex) {
        std::cerr << "ERROR: " << ex.what() << '\n';
    }
}

