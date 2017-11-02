// partition.cpp                                                      -*-C++-*-
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

#include "slides-partition.hpp"
#include "lomuto_partition.hpp"
#include "hoare_partition.hpp"
#include "parallel_partition.hpp"
#include "timer.hpp"
#include <algorithm>
#include <exception>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>
#include <utility>
#include <vector>

// ----------------------------------------------------------------------------

template <typename Partition, typename Container, typename Predicate>
auto test(Partition partition, Container container, Predicate predicate)
    -> std::pair<utility::timer_duration, bool> {
    utility::timer timer;
    timer.start();
    auto it = partition(container.begin(), container.end(), predicate);
    auto time = timer.stop();
    bool rc = std::is_partitioned(container.begin(), container.end(), predicate)
        && it == std::partition_point(container.begin(), container.end(), predicate)
        ;
    return { time, rc };
}

// ----------------------------------------------------------------------------

template <typename Partition, typename Container, typename Predicate>
void test(std::string const& prefix, std::string const& name, Partition partition,
          Container const& container, Predicate predicate) {
#ifdef CXX17
    auto[time, rc] = test(partition, container, predicate);
    std::cout << std::setw(60) << name << ' '
              << (rc? "passed": "\x1b[31mfailed\x1b[0m") << ' '
              << time << ' '
              << '\n' << std::flush;
#elif 0
    auto p = test(partition, container, predicate);
    std::cout << std::setw(60) << name << ' '
              << (p.second? "passed": "\x1b[31mfailed\x1b[0m") << ' '
              << p.first << ' '
              << '\n' << std::flush;
#else
    auto p = test(partition, container, predicate);
    std::cout << prefix
              << "\"name\"=\"" << name << "\", "
              << "\"time\"=\"" << p.first << "\", "
              << "\"result\"=\"" << (p.second? "passed": "failed") << "\" "
              << "},\n" << std::flush;
#endif
}

// ----------------------------------------------------------------------------

template <typename Predicate>
void run_test(std::string const& prefix, std::vector<int> const& v, Predicate predicate)
{
#if 1
    test(prefix, "std::partition", [](auto begin, auto end, auto pred) {
            return std::partition(begin, end, pred);
        }, v, predicate);
    test(prefix, "blocked", [](auto begin, auto end, auto pred) {
            return blocked(begin, end, pred);
        }, v, predicate);
#endif
    {
        thread_pool pool1(std::thread::hardware_concurrency());
    test(prefix, "blocked(pool)", [&pool1](auto begin, auto end, auto pred) {
            return blocked(pool1, begin, end, pred);
        }, v, predicate);
    }
#if 1
    //test(prefix, "std::partition", [](auto begin, auto end, auto pred) {
    //        return std::partition(begin, end, pred);
    //    }, v, predicate);
    //test(prefix, "lomuto_partition1", [](auto begin, auto end, auto pred) {
    //        return lomuto_partition1(begin, end, pred);
    //    }, v, predicate);
    //test(prefix, "lomuto_partition2", [](auto begin, auto end, auto pred) {
    //        return lomuto_partition2(begin, end, pred);
    //    }, v, predicate);
    test(prefix, "lomuto_partition3", [](auto begin, auto end, auto pred) {
            return lomuto_partition3(begin, end, pred);
        }, v, predicate);
    test(prefix, "hoare_partition", [](auto begin, auto end, auto pred) {
            return hoare_partition(begin, end, pred);
        }, v, predicate);
    //test(prefix, "blocked_partition", [](auto begin, auto end, auto pred) {
    //        return blocked_partition(begin, end, pred);
    //    }, v, predicate);
    //test(prefix, "algorithm_partition", [](auto begin, auto end, auto pred) {
    //        return algorithm_partition(begin, end, pred);
    //    }, v, predicate);
    //test(prefix, "sentinel_partition", [](auto begin, auto end, auto pred) {
    //        return sentinel_partition(begin, end, pred);
    //    }, v, predicate);
    // test(prefix, "parallel_partition", [](auto begin, auto end, auto pred) {
    //         return parallel_partition(begin, end, pred);
    //     }, v, predicate);
#endif
    nstd::thread_pool pool(std::thread::hardware_concurrency());
    pool.start();
#if 0
    test(prefix, "parallel_partition<nstd::block_manager>",
         nstd::parallel_partition<nstd::block_manager>(pool), v, predicate);
    test(prefix, "parallel_partition2<nstd::block_manager>",
         nstd::parallel_partition2<nstd::block_manager>(pool), v, predicate);
    test(prefix, "parallel_partition3<nstd::block_manager>",
         nstd::parallel_partition3<nstd::block_manager>(pool), v, predicate);
    test(prefix, "parallel_partition<nstd::block_manager_atomic>",
         nstd::parallel_partition<nstd::block_manager_atomic>(pool), v, predicate);
    test(prefix, "parallel_partition2<nstd::block_manager_atomic>",
         nstd::parallel_partition2<nstd::block_manager_atomic>(pool), v, predicate);
    test(prefix, "parallel_partition3<nstd::block_manager_atomic>",
         nstd::parallel_partition3<nstd::block_manager_atomic>(pool), v, predicate);
    test(prefix, "parallel_partition<nstd::block_manager_padded_atomic>",
         nstd::parallel_partition<nstd::block_manager_padded_atomic>(pool), v, predicate);
    test(prefix, "parallel_partition2<nstd::block_manager_padded_atomic>",
         nstd::parallel_partition2<nstd::block_manager_padded_atomic>(pool), v, predicate);
    test(prefix, "parallel_partition3<nstd::block_manager_padded_atomic>",
         nstd::parallel_partition3<nstd::block_manager_padded_atomic>(pool), v, predicate);
#endif
#if 1
    //test(prefix, "parallel_partition<nstd::block_manager_relaxed>",
    //     nstd::parallel_partition<nstd::block_manager_relaxed>(pool), v, predicate);
    test(prefix, "parallel_partition2<nstd::block_manager_relaxed>",
         nstd::parallel_partition2<nstd::block_manager_relaxed>(pool), v, predicate);
    //test(prefix, "parallel_partition3<nstd::block_manager_relaxed>",
    //     nstd::parallel_partition3<nstd::block_manager_relaxed>(pool), v, predicate);
#endif
}

// ----------------------------------------------------------------------------

void run_tests(int size)
{
    std::minstd_rand rnd(0);
    std::vector<int> v;
    // std::cout << "generating\n" << std::flush;
    std::generate_n(std::back_inserter(v), size, [size,&rnd]{ return rnd() % size; });

    for (auto divisor: { 2, 5, 10, 20, 100, 1000 }) {
#if 0
        std::cout << "--- size=" << size << " divisor=" << divisor << '\n';
#else
        std::ostringstream prefix;
        prefix << "{ "
               << "\"size\"=" << size << ", "
               << "\"divisor\"=" << divisor << ", ";
#endif
        auto predicate([size, divisor](auto const& value){ return value < size / divisor; });
        run_test(prefix.str(), v, predicate);
    }
}

// ----------------------------------------------------------------------------

int main(int ac, char*[]) {
    if (ac != 1) {
        int array[] = {
             1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
            11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        };
        std::copy(std::begin(array), std::end(array), std::ostream_iterator<int>(std::cout, " "));
        std::cout << "\n";
        auto x = blocked(std::begin(array), std::end(array), [](int value){ return value % 2; });
        std::copy(std::begin(array), x, std::ostream_iterator<int>(std::cout, " "));
        std::cout << "\n";
        std::copy(x, std::end(array), std::ostream_iterator<int>(std::cout, " "));
        std::cout << "\n";
        return 0;
    }
    try {
        for (auto size: {
                1000ull,
                    10000ull,
                    100000ull,
                    1000000ull,
                    10000000ull,
                    100000000ull,
#if 1
                    1000000000ull,
#endif
                    }) {
	    for (int factor:{ 1, 2, 5 }) {
                run_tests(size * factor);
            }
        }
    }
    catch (std::exception const& ex) {
        std::cerr << "ERROR: " << ex.what() << '\n';
    }
}

