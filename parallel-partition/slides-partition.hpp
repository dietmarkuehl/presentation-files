// slides-partition.hpp                                               -*-C++-*-
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

#ifndef INCLUDED_SLIDES_PARTITION
#define INCLUDED_SLIDES_PARTITION

#include "latch.hpp"
#include "not_fn.hpp"
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <tuple>
#include <utility>
#include <list>
#include <vector>

// ----------------------------------------------------------------------------

template <typename RndIt>
class Block {
    RndIt d_begin;
    RndIt d_current;
    RndIt d_end;
public:
    Block(): d_begin(), d_current(), d_end() {}
    Block(RndIt b, RndIt e): d_begin(b), d_current(b), d_end(e) {}
    bool empty() const { return this->d_current == this->d_end; }
    auto operator*() const -> decltype(*this->d_current) {
        return *this->d_current;
    }
    Block& operator++() { ++this->d_current; return *this; }

    RndIt begin() const   { return this->d_begin; }
    RndIt current() const { return this->d_current; }
    RndIt cur() const { return this->d_current; }
    RndIt end() const     { return this->d_end; }
};

template <typename Blk, typename Predicate>
std::pair<Blk, Blk> block(Blk f, Blk b, Predicate pred)
{
    while (true) {
        while (!f.empty() && pred(*f)) {
            ++f;
        }
        while (!b.empty() && !pred(*b)) {
            ++b;
        }
        if (f.empty() || b.empty()) {
            return std::make_pair(f, b);
        }
        using std::swap; swap(*f, *b);
    }
}

template <typename It, typename Pred>
It clean_up(Block<It> f, Block<It> b, Pred pred) {
    if (b.empty()) {
        return std::partition(f.current(), f.end(), pred);
    }
    auto s = b.current();
    auto p = std::partition(b.current(), b.end(), pred);
    std::swap_ranges(s, p, b.begin());
    return b.begin() + (p - s);
}

template <typename RndIt, int Size = 4096>
class BlockQueue {
    RndIt beg, end;
    static constexpr int bs = Size;
    std::atomic<int> size, f_off{0}, b_off{0};
public:
    BlockQueue(RndIt b, RndIt e): beg(b), end(e), size(e-b) {}
    Block<RndIt> front() {
        auto s  = size.fetch_sub(bs); 
        s = std::min(std::max(0, s), bs);
        auto off = f_off.fetch_add(s);
        return Block<RndIt>(beg + off, beg + off + s);
    }
    Block<RndIt> back() {
        auto s = size.fetch_sub(bs); 
        s = std::min(std::max(0, s), bs);
        auto off = b_off.fetch_add(s);
        return Block<RndIt>(end - off - s, end - off);
    }
    RndIt midpoint(){ return beg + f_off; }
};

// ----------------------------------------------------------------------------

template <typename RndIt, typename Predicate>
RndIt blocked(RndIt begin, RndIt end, Predicate pred) {
    BlockQueue<RndIt> q(begin, end);
    Block<RndIt>      f, b;
    while (true) {
        if (f.empty() && (f = q.front()).empty()) { break; }
        if (b.empty() && (b = q.back()).empty()) { break; }
        std::tie(f, b) = block(f, b, pred);
    }
    return clean_up(f, b, pred);
}

// ----------------------------------------------------------------------------

struct join_thread
    : std::thread {
    using std::thread::thread;
    ~join_thread(){ this->join(); }
};

class thread_pool {
    mutable std::mutex                d_mutex;
    std::condition_variable           d_condition;
    std::deque<std::function<void()>> d_jobs;
    std::list<join_thread>            d_threads;
    void run() {
        for (std::function<void()> fun([]{}); fun; ) {
            fun();
            std::unique_lock<std::mutex> kerberos(this->d_mutex);
            this->d_condition.wait(kerberos,
                                   [this]{ return !this->d_jobs.empty(); });
            fun = std::move(this->d_jobs.front());
            this->d_jobs.pop_front();
        }
    }
public:
    explicit thread_pool(int count) {
        while (count--){ this->d_threads.emplace_back([this]{ run(); }); }
    }
    ~thread_pool(){ this->stop(); }
    void stop(){
        {
            std::lock_guard<std::mutex> kerberos(this->d_mutex);
            for (std::size_t i(0); i != this->d_threads.size(); ++i) {
                this->d_jobs.emplace_back(std::function<void()>());
            }
        }
        this->d_condition.notify_all();
        this->d_threads.clear();
    }
    template <typename Job>
    void enqueue_job(Job job) {
        std::lock_guard<std::mutex> kerberos(this->d_mutex);
        this->d_jobs.emplace_back(std::move(job));
        this->d_condition.notify_one();
    }
    std::size_t size() const {
        std::lock_guard<std::mutex> kerberos(this->d_mutex);
        return this->d_threads.size();
    }
};

// ----------------------------------------------------------------------------

template <typename RndIt1, typename RndIt2>
void swap_ranges_helper(RndIt1 beg1, RndIt1 end1, RndIt2 beg2) {
    auto size = (beg2 < end1? beg2: end1) - beg1;
    auto end2 = beg2 + (end1 - beg1);
    std::swap_ranges(beg1, beg1 + size, end2 - size);
}

// ----------------------------------------------------------------------------

template <typename RndIt, typename Predicate>
RndIt blocked(thread_pool& p, RndIt begin, RndIt end, Predicate pred) {
    BlockQueue<RndIt>         q(begin, end);
    std::vector<Block<RndIt>> remain(p.size());
    nstd::latch               l(remain.size());

    auto job = [&q, pred](auto& remain){
        remain = [&q, pred]()->Block<RndIt>{
            for (Block<RndIt> f, b;;) {
                if (f.empty() && (f = q.front()).empty())
                    return  { b.cur(), std::partition(b.cur(), b.end(), pred) };
                if (b.empty() && (b = q.back()).empty())
                    return { std::partition(f.cur(), f.end(), pred), f.end() };
                std::tie(f, b) = block(f, b, pred);
            }
        }();
    };

    for (auto& block: remain) {
        p.enqueue_job([job, &l, &block](){ job(block); l.arrive(); });
    }
    l.wait();

    RndIt mid = q.midpoint();
    auto rp = std::partition(remain.begin(), remain.end(),
                             [mid](auto& b){ return b.begin() < mid; });
    std::sort(remain.begin(), rp,
              [](auto& b0, auto& b1){ return b1.begin() < b0.begin(); });
    for (auto it(remain.begin()); it != rp; ++it) {
        mid -= it->end() - it->begin();
        swap_ranges_helper(it->begin(), it->end(), mid);
    }
    std::sort(rp, remain.end(),
              [](auto& b0, auto& b1){ return b0.begin() < b1.begin(); });
    for (auto it(rp); it != remain.end(); ++it) {
        swap_ranges_helper(mid, mid + (it->end() - it->begin()), it->begin());
        mid += it->end() - it->begin();
    }
    return mid;
}

// ----------------------------------------------------------------------------

#endif
