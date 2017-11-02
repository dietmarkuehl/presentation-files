// parallel_partition.hpp                                             -*-C++-*-
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

#ifndef INCLUDED_PARALLEL_PARTITION
#define INCLUDED_PARALLEL_PARTITION

#include "not_fn.hpp"
#include "latch.hpp"
#include "thread_pool.hpp"
#include "block_manager.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <mutex>
#include <thread>

// ----------------------------------------------------------------------------

namespace nstd {
    template <template <typename, int> class BlockManager>
    class parallel_partition;
    template <template <typename, int> class BlockManager>
    class parallel_partition2;
    template <template <typename, int> class BlockManager>
    class parallel_partition3;
}

// ----------------------------------------------------------------------------

template <typename RndIt>
struct parallel_partition_context {
    using difference_type = typename std::iterator_traits<RndIt>::difference_type;

    std::mutex      d_mutex;
    RndIt           d_begin;
    RndIt           d_end;
    difference_type d_block_size;

    parallel_partition_context(RndIt begin, RndIt end)
        : d_begin(begin)
        , d_end(end)
        , d_block_size(std::max(difference_type(128),
                                std::distance(begin, end)
                                / (8 * std::thread::hardware_concurrency()))) {
    }
    
    std::pair<RndIt, RndIt> pop_front() {
        std::lock_guard<std::mutex> kerberos(this->d_mutex);
        auto size = std::min(std::distance(this->d_begin, this->d_end), this->d_block_size);
        std::pair<RndIt, RndIt> rc(this->d_begin, this->d_begin + size);
        this->d_begin += size;
        return rc;
    }
    std::pair<RndIt, RndIt> pop_back() {
        std::lock_guard<std::mutex> kerberos(this->d_mutex);
        auto size = std::min(std::distance(this->d_begin, this->d_end), this->d_block_size);
        std::pair<RndIt, RndIt> rc(this->d_end - size, this->d_end);
        this->d_end -= size;
        return rc;
    }
};

template <typename RndIt, typename Predicate>
std::pair<RndIt, RndIt> block_partition(RndIt fit, RndIt fend,
                                        RndIt bit, RndIt bend,
                                        Predicate predicate) {
    while (true) {
        while (fit != fend &&  predicate(*fit)) { ++fit; }
        while (bit != bend && !predicate(*bit)) { ++bit; }
        if (fit == fend || bit == bend) { return std::make_pair(fit, bit); }
        std::iter_swap(fit, bit);
    }
}

template <typename RndIt, typename Predicate>
RndIt blocked_partition(RndIt begin, RndIt end, Predicate pred) {
    parallel_partition_context<RndIt> context(begin, end);
    std::pair<RndIt, RndIt> front(begin, begin), back(end, end);
    auto bbeg(end);
    while (true) {
        if (front.first == front.second) {
            front = context.pop_front();
            if (front.first == front.second) {
                auto rbeg = back.first;
                auto mid =  std::partition(back.first, back.second, pred);
                std::swap_ranges(rbeg, mid, bbeg);
                return bbeg + std::distance(rbeg, mid);
            }
        }
        if (back.first == back.second) {
            back = context.pop_back();
            bbeg = back.first;
            if (back.first == back.second) {
                return std::partition(front.first, front.second, pred);
            }
        }
        std::tie(front.first, back.first) = block_partition(front.first, front.second,
                                                            back.first, back.second, pred);
    }
    return begin;
}

// ----------------------------------------------------------------------------

template <template <typename, int> class BlockManager>
class nstd::parallel_partition {
private:
    nstd::thread_pool& d_pool;
    static constexpr int blocksize = 1024;
    static constexpr int minblocks = 4;

public:
    explicit parallel_partition(nstd::thread_pool& pool): d_pool(pool) {}
    template <typename RndIt, typename Predicate>
    RndIt operator()(RndIt begin, RndIt end, Predicate predicate) const;
};

template <template <typename, int> class BlockManager>
template <typename RndIt, typename Predicate>
RndIt nstd::parallel_partition<BlockManager>::operator()(RndIt begin, RndIt end, Predicate predicate) const {
    auto len = std::distance(begin, end);
    if (len < minblocks * blocksize) {
        return std::partition(begin, end, predicate);
    }
    BlockManager<RndIt, blocksize> bm(begin, end);
    int maxjobs = this->d_pool.thread_count() / 2;
    std::vector<std::pair<RndIt, RndIt>> leftover(maxjobs);
    
    nstd::latch latch(maxjobs);
    auto job = [&](auto& lastblock){
        [&]{
            auto front = bm.pop_front();
            auto back  = bm.pop_back();
            auto fit   = front.first;
            auto bit   = back.first;
            while (true) {
                while (front.second == (fit = std::find_if(fit, front.second, nstd::not_fn(predicate)))) {
                    front = bm.pop_front();
                    fit   = front.first;
                    if (front.first == front.second){
                        auto it = std::partition(back.first, back.second, predicate);
                        lastblock = std::make_pair(back.first, it);
                        return;
                    }
                }
                while (back.second == (bit = std::find_if(bit, back.second, predicate))) {
                    back = bm.pop_back();
                    bit  = back.first;
                    if (back.first == back.second) {
                        auto it = std::partition(fit, front.second, predicate);
                        lastblock = std::make_pair(it, front.second);
                        return;
                    }
                }
                std::iter_swap(fit, bit);
                ++fit;
                ++bit;
            }
        }(); // NOTE: there is a call here!
        
        latch.arrive();
    };
    for (int j = 0; j != maxjobs; ++j) {
        this->d_pool.enqueue_job([&,j]{ job(leftover[j]); });
    }
    latch.wait();

    auto p = std::minmax_element(leftover.begin(), leftover.end());
    begin = p.first->first;
    end   = p.second->second;
    return std::partition(begin, end, predicate); //-dk:TODO swap blocks

    return begin;
}

// ----------------------------------------------------------------------------

template <template <typename, int> class BlockManager>
class nstd::parallel_partition2 {
private:
    nstd::thread_pool& d_pool;
    static constexpr int blocksize = 1024;
    static constexpr int minblocks = 4;

public:
    explicit parallel_partition2(nstd::thread_pool& pool): d_pool(pool) {}
    template <typename RndIt, typename Predicate>
    RndIt operator()(RndIt begin, RndIt end, Predicate predicate) const;
};

template <template <typename, int> class BlockManager>
template <typename RndIt, typename Predicate>
RndIt nstd::parallel_partition2<BlockManager>::operator()(RndIt begin, RndIt end, Predicate predicate) const {
    auto len = std::distance(begin, end);
    if (len < minblocks * blocksize) {
        return std::partition(begin, end, predicate);
    }
    BlockManager<RndIt, blocksize> bm(begin, end);
    int maxjobs = this->d_pool.thread_count();
    std::vector<std::pair<RndIt, RndIt>> leftover(maxjobs);
    
    nstd::latch latch(maxjobs);
    auto job = [&](auto& lastblock){
        [&]{
            auto front = bm.pop_front();
            auto back  = bm.pop_back();
            auto fit   = front.first;
            auto bit   = back.first;
            while (true) {
                while (front.second == (fit = std::find_if(fit, front.second, nstd::not_fn(predicate)))) {
                    front = bm.pop_front();
                    fit   = front.first;
                    if (front.first == front.second){
                        auto it = std::partition(back.first, back.second, predicate);
                        lastblock = std::make_pair(back.first, it);
                        return;
                    }
                }
                while (back.second == (bit = std::find_if(bit, back.second, predicate))) {
                    back = bm.pop_back();
                    bit  = back.first;
                    if (back.first == back.second) {
                        auto it = std::partition(fit, front.second, predicate);
                        lastblock = std::make_pair(it, front.second);
                        return;
                    }
                }
                std::iter_swap(fit, bit);
                ++fit;
                ++bit;
            }
        }(); // NOTE: there is a call here!
        
        latch.arrive();
    };
    for (int j = 0; j != maxjobs; ++j) {
        this->d_pool.enqueue_job([&,j]{ job(leftover[j]); });
    }
    latch.wait();

    auto midpoint = bm.midpoint();
    std::sort(leftover.begin(), leftover.end());
    auto leftend = std::partition_point(leftover.begin(), leftover.end(),
                                        [=](auto&& p){ return p.first < midpoint; });

    auto rightbegin = leftend;
    auto leftbegin  = leftover.begin();
    auto rightend   = leftover.end();
    while (leftend != leftbegin) {
        --leftend;
        auto size = std::min(std::distance(leftend->first, leftend->second),
                             std::distance(leftend->second, midpoint));
        std::swap_ranges(leftend->first, leftend->first + size, midpoint - size);
        midpoint -= std::distance(leftend->first, leftend->second);
    }
    while (rightbegin != rightend) {
        auto size = std::min(std::distance(rightbegin->first, rightbegin->second),
                             std::distance(midpoint, rightbegin->first));
        std::swap_ranges(midpoint, midpoint + size, rightbegin->second - size);
        midpoint += std::distance(rightbegin->first, rightbegin->second);
        ++rightbegin;
    }
    return midpoint;
}

// ----------------------------------------------------------------------------

template <template <typename, int> class BlockManager>
class nstd::parallel_partition3 {
private:
    nstd::thread_pool& d_pool;
    static constexpr int blocksize = 1024;
    static constexpr int minblocks = 4;

public:
    explicit parallel_partition3(nstd::thread_pool& pool): d_pool(pool) {}
    template <typename RndIt, typename Predicate>
    RndIt operator()(RndIt begin, RndIt end, Predicate predicate) const;
};

template <template <typename, int> class BlockManager>
template <typename RndIt, typename Predicate>
RndIt nstd::parallel_partition3<BlockManager>::operator()(RndIt begin, RndIt end, Predicate predicate) const {
    auto len = std::distance(begin, end);
    if (len < minblocks * blocksize) {
        return std::partition(begin, end, predicate);
    }
    BlockManager<RndIt, blocksize> bm(begin, end);
    int maxjobs = this->d_pool.thread_count();
    std::vector<std::pair<RndIt, RndIt>> leftover(maxjobs);
    
    nstd::latch latch(maxjobs);
    auto job = [&](auto& lastblock){
        [&]{
            auto front = bm.pop_front();
            auto back  = bm.pop_back();
            auto fit   = front.first;
            auto bit   = back.first;
            while (true) {
#if 0
                while (front.second == (fit = std::find_if(fit, front.second, nstd::not_fn(predicate)))) {
                    front = bm.pop_front();
                    fit   = front.first;
                    if (front.first == front.second){
                        auto it = std::partition(back.first, back.second, predicate);
                        lastblock = std::make_pair(back.first, it);
                        return;
                    }
                }
#else
            find_front:
                while (fit != front.second && predicate(*fit)) {
                    ++fit;
                }
                if (front.second == fit) {
                    front = bm.pop_front();
                    if (front.first == front.second){
                        auto it = std::partition(back.first, back.second, predicate);
                        lastblock = std::make_pair(back.first, it);
                        return;
                    }
                    fit   = front.first;
                    goto find_front;
                }
#endif
#if 0
                while (back.second == (bit = std::find_if(bit, back.second, predicate))) {
                    back = bm.pop_back();
                    bit  = back.first;
                    if (back.first == back.second) {
                        auto it = std::partition(fit, front.second, predicate);
                        lastblock = std::make_pair(it, front.second);
                        return;
                    }
                }
#else
            find_back:
                while (bit != back.second && !predicate(*bit)) {
                    ++bit;
                }
                if (bit == back.second) {
                    back = bm.pop_back();
                    if (back.first == back.second) {
                        auto it = std::partition(fit, front.second, predicate);
                        lastblock = std::make_pair(it, front.second);
                        return;
                    }
                    bit  = back.first;
                    goto find_back;
                }
#endif
                std::iter_swap(fit, bit);
                ++fit;
                ++bit;
            }
        }(); // NOTE: there is a call here!
        
        latch.arrive();
    };
    for (int j = 0; j != maxjobs; ++j) {
        this->d_pool.enqueue_job([&,j]{ job(leftover[j]); });
    }
    latch.wait();

    auto midpoint = bm.midpoint();
    std::sort(leftover.begin(), leftover.end());
    auto leftend = std::partition_point(leftover.begin(), leftover.end(),
                                        [=](auto&& p){ return p.first < midpoint; });

    auto rightbegin = leftend;
    auto leftbegin  = leftover.begin();
    auto rightend   = leftover.end();
    while (leftend != leftbegin) {
        --leftend;
        auto size = std::min(std::distance(leftend->first, leftend->second),
                             std::distance(leftend->second, midpoint));
        std::swap_ranges(leftend->first, leftend->first + size, midpoint - size);
        midpoint -= std::distance(leftend->first, leftend->second);
    }
    while (rightbegin != rightend) {
        auto size = std::min(std::distance(rightbegin->first, rightbegin->second),
                             std::distance(midpoint, rightbegin->first));
        std::swap_ranges(midpoint, midpoint + size, rightbegin->second - size);
        midpoint += std::distance(rightbegin->first, rightbegin->second);
        ++rightbegin;
    }
    return midpoint;
}

// ----------------------------------------------------------------------------

#endif
