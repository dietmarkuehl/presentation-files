// parallel_sort.hpp                                                  -*-C++-*-
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

#ifndef INCLUDED_PARALLEL_SORT
#define INCLUDED_PARALLEL_SORT

#include "thread_pool.hpp"
#include "latch.hpp"
#include "block_manager.hpp"
#include "parallel_partition.hpp"
#include <algorithm>
#include <atomic>
#include <memory>
#include <utility>

// ----------------------------------------------------------------------------

#if 0
struct frankensort4 {
    nstd::thread_pool& d_pool;
    frankensort4(nstd::thread_pool& pool): d_pool(pool) {}

    template <typename It, typename Comp>
    void operator()(It begin, It end, Compare compare) const {
        nstd::latch latch(1);
        (*this)([&latch]{ latch.arrive(); }, begin, end, compare);
        latch.wait();
    }
};
#endif

// ----------------------------------------------------------------------------

template <template<typename, int> class BlockManager, typename Continuation, typename RndIt, typename Compare>
void async_sort_with(nstd::thread_pool& pool, Continuation continuation, RndIt begin, RndIt end, Compare compare) {
    struct control
        : std::enable_shared_from_this<control> {
        nstd::thread_pool& pool;
        Continuation       continuation;
        Compare            compare;
        std::atomic<int>   active;

        control(nstd::thread_pool& pool, Continuation continuation, Compare compare)
            : pool(pool)
            , continuation(std::move(continuation))
            , compare(compare)
            , active(0) {
        }
        void do_it(RndIt begin, RndIt end) {
            auto size = std::distance(begin, end);
            if (size < 8000) {
                std::sort(begin, end, this->compare);
                return this->clean_up();
            }

            auto mid = begin + size / 2;
            std::iter_swap(mid, end - 1);
            auto partition_pred = [=, pivot=*(end - 1)](auto const& value) {
                return this->compare(value, pivot);
            };
            auto partition_point = nstd::parallel_partition<BlockManager>(pool)(begin, end - 1, partition_pred);
            std::iter_swap(end - 1, partition_point);
            if (begin != partition_point) {
                ++this->active;
#if 0
                auto self = this->shared_from_this();
                pool.enqueue_job([self, begin, end=partition_point]{ self->do_it(begin, end); });
#else
                this->do_it(begin, partition_point);
#endif
            }
            ++this->active;
            this->do_it(partition_point + 1, end);
            this->clean_up();
        }
        void clean_up() {
            if (0 != --this->active) {
                return;
            }
            this->continuation();
        }
    };
    
    auto ctrl = std::make_shared<control>(pool, std::move(continuation), compare);
    ++ctrl->active;
#if 1
    pool.enqueue_job([ctrl, begin, end]{ ctrl->do_it(begin, end); });
#else
    ctrl->do_it(begin, end);
#endif
}

template <template<typename, int> class BlockManager>
struct parallel_sort_with_async {
    nstd::thread_pool& d_pool;
    parallel_sort_with_async(nstd::thread_pool& pool): d_pool(pool) {}

    template <typename It, typename Compare>
    void operator()(It begin, It end, Compare compare) const {
        nstd::latch latch(1);
        async_sort_with<BlockManager>(this->d_pool, [&]{ latch.arrive(); },
                                      begin, end, compare);
                                      
        latch.wait();
    }
};

// ----------------------------------------------------------------------------

#endif
