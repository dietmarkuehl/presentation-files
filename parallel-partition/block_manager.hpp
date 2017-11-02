// block_manager.hpp                                                  -*-C++-*-
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

#ifndef INCLUDED_BLOCK_MANAGER
#define INCLUDED_BLOCK_MANAGER

#include <atomic>
#include <mutex>
#include <algorithm>
#include <cstddef>
#include <cassert>

// ----------------------------------------------------------------------------

namespace nstd {
    template <typename RndIt, int Size> class block_manager;
    template <typename RndIt, int Size> class block_manager_atomic;
    template <typename RndIt, int Size> class block_manager_padded_atomic;
    template <typename RndIt, int Size> class block_manager_relaxed;
}

// ----------------------------------------------------------------------------

template <typename RndIt, int Size>
class nstd::block_manager {
private:
    RndIt      d_begin;
    RndIt      d_end;
    std::mutex d_mutex;

public:
    explicit block_manager(RndIt begin, RndIt end)
        : d_begin(begin)
        , d_end(end)
        , d_mutex() {
    }
    auto midpoint() -> RndIt { return this->d_begin; }
    auto pop_front() -> std::pair<RndIt, RndIt> {
        std::lock_guard<std::mutex> kerberos(this->d_mutex);
        auto end = this->d_begin + std::min(Size, int(std::distance(this->d_begin, d_end)));
        auto rc = std::make_pair(this->d_begin, end);
        this->d_begin = end;
        return rc;
    }
    auto pop_back() -> std::pair<RndIt, RndIt> {
        std::lock_guard<std::mutex> kerberos(this->d_mutex);
        auto begin = this->d_end - std::min(Size, int(std::distance(this->d_begin, d_end)));
        auto rc = std::make_pair(begin, this->d_end);
        this->d_end = begin;
        return rc;
    }
};

// ----------------------------------------------------------------------------

template <typename RndIt, int Size>
class nstd::block_manager_atomic {
private:
    RndIt            d_begin;
    RndIt            d_end;
    std::atomic<int> d_size_left;
    std::atomic<int> d_offset_front;
    std::atomic<int> d_offset_back;
    int elements_remaining() {
        int result = this->d_size_left -= Size;
        if (0 <= result)    { return Size; } // Size elements obtained
        return std::max(0, Size + result);
    }
public:
    explicit block_manager_atomic(RndIt begin, RndIt end)
        : d_begin(begin)
        , d_end(end)
        , d_size_left(std::distance(this->d_begin, this->d_end))
        , d_offset_front(0)
        , d_offset_back(0) {
    }
    auto midpoint() const { return this->d_begin + this->d_offset_front; }
    auto pop_front() -> std::pair<RndIt, RndIt> {
        int elements = this->elements_remaining();
        int offset   = this->d_offset_front += elements;
        return std::make_pair(this->d_begin + offset - elements,
                              this->d_begin + offset);
    }
    auto pop_back() -> std::pair<RndIt, RndIt> {
        int elements = this->elements_remaining();
        int offset   = this->d_offset_back += elements;
        return std::make_pair(this->d_end - offset,
                              this->d_end - offset + elements);
    }
};

// ----------------------------------------------------------------------------

template <typename RndIt, int Size>
class nstd::block_manager_padded_atomic {
private:
    RndIt            d_begin;
    RndIt            d_end;
    alignas(64) std::atomic<int> d_size_left;
    alignas(64) std::atomic<int> d_offset_front;
    alignas(64) std::atomic<int> d_offset_back;

    int elements_remaining() {
        int result = this->d_size_left -= Size;
        if (0 <= result)    { return Size; } // Size elements obtained
        return std::max(0, Size + result);
    }
public:
    explicit block_manager_padded_atomic(RndIt begin, RndIt end)
        : d_begin(begin)
        , d_end(end)
        , d_size_left(std::distance(this->d_begin, this->d_end))
        , d_offset_front(0u)
        , d_offset_back(0u) {
    }
    auto midpoint() const { return this->d_begin + this->d_offset_front; }
    auto pop_front() -> std::pair<RndIt, RndIt> {
        int elements = this->elements_remaining();
        int offset   = d_offset_front += elements;
        return std::make_pair((this->d_begin + offset) - elements,
                              this->d_begin + offset);
    }
    auto pop_back() -> std::pair<RndIt, RndIt> {
        int elements = this->elements_remaining();
        int offset   = d_offset_back += elements;
        return std::make_pair(this->d_end - offset,
                              (this->d_end - offset) + elements);
    }
};

// ----------------------------------------------------------------------------

template <typename RndIt, int Size>
class nstd::block_manager_relaxed {
private:
    RndIt            d_begin;
    RndIt            d_end;
    alignas(64) std::atomic<int> d_size_left;
    alignas(64) std::atomic<int> d_offset_front;
    alignas(64) std::atomic<int> d_offset_back;

    int elements_remaining() {
        int result   = d_size_left.fetch_sub(Size, std::memory_order_relaxed);
        result -= Size;
        if (0 <= result)    { return Size; } // Size elements obtained
        return std::max(0, Size + result);
    }
public:
    explicit block_manager_relaxed(RndIt begin, RndIt end)
        : d_begin(begin)
        , d_end(end)
        , d_size_left(std::distance(this->d_begin, this->d_end))
        , d_offset_front(0u)
        , d_offset_back(0u) {
    }
    auto midpoint() const { return this->d_begin + this->d_offset_front; }
    auto pop_front() -> std::pair<RndIt, RndIt> {
        int elements = this->elements_remaining();
        int offset   = d_offset_front.fetch_add(elements, std::memory_order_relaxed);
        offset += elements;
        return std::make_pair((this->d_begin + offset) - elements,
                              this->d_begin + offset);
    }
    auto pop_back() -> std::pair<RndIt, RndIt> {
        int elements = this->elements_remaining();
        int offset   = d_offset_back.fetch_add(elements, std::memory_order_relaxed);
        offset += elements;
        return std::make_pair(this->d_end - offset,
                              (this->d_end - offset) + elements);
    }
};

// ----------------------------------------------------------------------------

#endif
