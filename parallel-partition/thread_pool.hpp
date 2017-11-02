// thread_pool.hpp                                                    -*-C++-*-
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

#ifndef INCLUDED_THREAD_POOL
#define INCLUDED_THREAD_POOL

#include <condition_variable>
#include <deque>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// ----------------------------------------------------------------------------

namespace nstd {
    class thread_pool;
}

// ----------------------------------------------------------------------------

class nstd::thread_pool {
private:
    int                               d_count;
    std::mutex                        d_mutex;
    std::condition_variable           d_condition;
    std::vector<std::thread>          d_threads;
    std::deque<std::function<void()>> d_jobs;
    bool                              d_running;

    bool process_job() {
        std::function<void()> fun;
        {
            std::unique_lock<std::mutex> kerberos(this->d_mutex);
            while (this->d_jobs.empty() && this->d_running) {
                this->d_condition.wait(kerberos);
            }
            if (this->d_jobs.empty()) {
                return false;
            }
            fun = std::move(this->d_jobs.front());
            this->d_jobs.pop_front();
        }
        fun();
        return true;
    }

public:
    explicit thread_pool(int count)
        : d_count(count)
        , d_running(false) {
        this->d_threads.reserve(count);
    }
    thread_pool(thread_pool&) = delete;
    void operator=(thread_pool&) = delete;
    ~thread_pool(){ this->stop(); }

    void start() {
        std::lock_guard<std::mutex> kerberos(this->d_mutex);
        this->d_running = true;
        for (int i(0); i != this->d_count; ++i) {
            this->d_threads.emplace_back(std::thread([this]{
                        while (true) {
                            try {
                                if (!this->process_job()) {
                                    break;
                                }
                            }
                            catch (std::exception const& ex) {
                                std::cerr << "ERROR: " << ex.what() << "\n";
                            }
                            catch (...) {
                                std::cerr << "ERROR: caught unkonwn error\n";
                            }
                        }
                    }));
        }
        
    }
    void stop() {
        bool stopping = true;
        {
            std::lock_guard<std::mutex> kerberos(this->d_mutex);
            if (this->d_running) {
                this->d_running = false;
                this->d_condition.notify_all();
            }
            else {
                stopping = false;
            }
        }
        if (stopping) {
            for (auto& thread: this->d_threads) {
                thread.join();
            }
        }
    }
    int thread_count() const { return this->d_count; }
    void enqueue_job(std::function<void()> job) {
        {
            std::lock_guard<std::mutex> kerberos(this->d_mutex);
            this->d_jobs.push_back(std::move(job));
        }
        this->d_condition.notify_one();
    }
};

// ----------------------------------------------------------------------------

#endif
