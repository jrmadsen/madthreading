// MIT License
//
// Copyright (c) 2017 Jonathan R. Madsen
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//


#include <UnitTest++.h>

#include <iomanip>
#include <iostream>

#include "madthreading/atomics/atomic.hh"
#include "madthreading/threading/auto_lock.hh"
#include "madthreading/threading/threading.hh"

#include <vector>


SUITE( Atomic_Tests )
{
    //------------------------------------------------------------------------//
    typedef unsigned short uint16;
    typedef unsigned int uint32;
    typedef unsigned long uint64;
    using mad::atomic;
    using mad::auto_lock;
    typedef mad::mutex  Mutex_t;
    //------------------------------------------------------------------------//

    //========================================================================//
    //
    //
    //      TESTS WITH DATATYPES
    //
    //
    //========================================================================//

#define NTHREADS_LC 16 // LC - lock check
#define NCYCLES_LC 10
    //========================================================================//
    template <typename _Tp>
    struct Basic
    {
        typedef _Tp value_type;
        value_type value;
        uint32 wait_until;
        atomic<uint32> counter;

        Basic()
        : value(value_type(0.)), wait_until(0), counter(0)
        { }

        Basic(uint32 _wait)
        : value(value_type(0.)), wait_until(_wait), counter(0)
        { }

#ifdef DEBUG
        bool wait() const { return (counter < wait_until) ? true : false; }
#else
        bool wait() const { return false; }
#endif
    };
    //========================================================================//
    void* increment_int_with_lock(void* threadarg)
    {
        struct Basic<uint32>* mydata
        = (struct Basic<uint32>*) threadarg;

        // try to create a data race
        mydata->counter++;
        while(mydata->wait()) { }

        static Mutex_t mtx;
        auto_lock lock(mtx);
        mydata->value += 1;
        return (void*) mydata;
    }
    //========================================================================//
    void* increment_atomic_int(void* threadarg)
    {
        struct Basic<atomic<uint32> >* mydata
        = (struct Basic<atomic<uint32> >*) threadarg;

        // try to create a data race
        mydata->counter++;
        while(mydata->wait()) { }

        mydata->value += 1;
        return (void*) mydata;
    }
    //========================================================================//
    void* pre_increment_atomic_int(void* threadarg)
    {
        struct Basic<atomic<uint32> >* mydata
        = (struct Basic<atomic<uint32> >*) threadarg;

        // try to create a data race
        mydata->counter++;
        while(mydata->wait()) { }

        ++mydata->value;
        return (void*) mydata;
    }
    //========================================================================//
    void* post_increment_atomic_int(void* threadarg)
    {
        struct Basic<atomic<uint32> >* mydata
        = (struct Basic<atomic<uint32> >*) threadarg;

        // try to create a data race
        mydata->counter++;
        while(mydata->wait()) { }

        mydata->value++;
        return (void*) mydata;
    }
    //========================================================================//
    void* increment_int_without_lock(void* threadarg)
    {
        struct Basic<atomic<uint32> >* mydata
        = (struct Basic<atomic<uint32> >*) threadarg;

        // try to create a data race
        mydata->counter++;
        while(mydata->wait()) { }

        mydata->value += 1;
        return (void*) mydata;
    }
    //========================================================================//
    void* increment_float_with_lock(void* threadarg)
    {
        struct Basic<double>* mydata
        = (struct Basic<double>*) threadarg;

        // try to create a data race
        mydata->counter++;
        while(mydata->wait()) { }

        static Mutex_t mtx;
        auto_lock lock(mtx);
        mydata->value += 1.0;
        return (void*) mydata;
    }
    //========================================================================//
    void* increment_atomic_float(void* threadarg)
    {
        struct Basic<atomic<double> >* mydata
        = (struct Basic<atomic<double> >*) threadarg;

        // try to create a data race
        mydata->counter++;
        while(mydata->wait()) { }

        mydata->value += 1.0;
        return (void*) mydata;
    }
    //========================================================================//
    void* increment_float_without_lock(void* threadarg)
    {
        struct Basic<atomic<double> >* mydata
        = (struct Basic<atomic<double> >*) threadarg;

        // try to create a data race
        mydata->counter++;
        while(mydata->wait()) { }

        mydata->value += 1.0;
        return (void*) mydata;
    }
    //========================================================================//

    //------------------------------------------------------------------------//
    TEST(Check_atomic_preincrement)
    {
        atomic<int> a = 0;
        int v = ++a;
        CHECK_EQUAL(1, v);
        CHECK_EQUAL(1, a);
    }
    //------------------------------------------------------------------------//
    TEST(Check_atomic_postincrement)
    {
        atomic<int> a = 0;
        int v = a++;
        CHECK_EQUAL(0, v);
        CHECK_EQUAL(1, a);
    }
    //------------------------------------------------------------------------//
    TEST(Check_int_POD_with_lock)
    {
        uint32 ncycles = NCYCLES_LC;
        const uint32 nthreads = NTHREADS_LC;

        for(uint32 j = 0; j < ncycles; ++j)
        {
            std::thread threads[nthreads];
            Basic<uint32> basic(nthreads);

            // have each thread run function
            for(uint32 i = 0; i < nthreads; ++i)
                threads[i] = std::move(std::thread(increment_int_with_lock,
                                 (void*) &basic));

            // ensure above is done before checking
            for(uint32 i = 0; i < nthreads; ++i)
                threads[i].join();

            CHECK_EQUAL(nthreads, basic.value);
        }
    }
    //------------------------------------------------------------------------//
    TEST(Check_Atomic_int_without_lock)
    {
        uint32 ncycles = NCYCLES_LC;
        const uint32 nthreads = NTHREADS_LC;

        for(uint32 j = 0; j < ncycles; ++j)
        {
            std::thread threads[nthreads];
            Basic<atomic<uint32> > basic(nthreads);

            // have each thread run function
            for(uint32 i = 0; i < nthreads; ++i)
                threads[i] = std::move(std::thread(increment_atomic_int,
                                 (void*) &basic));

            // ensure above is done before checking
            for(uint32 i = 0; i < nthreads; ++i)
                threads[i].join();

            CHECK_EQUAL(nthreads, basic.value);
        }
    }
    //------------------------------------------------------------------------//
    TEST(Check_Atomic_preincr_int_without_lock)
    {
        uint32 ncycles = NCYCLES_LC;
        const uint32 nthreads = NTHREADS_LC;

        for(uint32 j = 0; j < ncycles; ++j)
        {
            std::thread threads[nthreads];
            Basic<atomic<uint32> > basic(nthreads);

            // have each thread run function
            for(uint32 i = 0; i < nthreads; ++i)
                threads[i] = std::move(std::thread(pre_increment_atomic_int,
                                 (void*) &basic));

            // ensure above is done before checking
            for(uint32 i = 0; i < nthreads; ++i)
                threads[i].join();

            CHECK_EQUAL(nthreads, basic.value);
        }
    }
    //------------------------------------------------------------------------//
    TEST(Check_Atomic_postincr_int_without_lock)
    {
        uint32 ncycles = NCYCLES_LC;
        const uint32 nthreads = NTHREADS_LC;

        for(uint32 j = 0; j < ncycles; ++j)
        {
            std::thread threads[nthreads];
            Basic<atomic<uint32> > basic(nthreads);

            // have each thread run function
            for(uint32 i = 0; i < nthreads; ++i)
                threads[i] = std::move(std::thread(post_increment_atomic_int,
                                 (void*) &basic));

            // ensure above is done before checking
            for(uint32 i = 0; i < nthreads; ++i)
                threads[i].join();

            CHECK_EQUAL(nthreads, basic.value);
        }
    }
    //------------------------------------------------------------------------//
    TEST(Check_float_POD_with_lock)
    {
        uint32 ncycles = NCYCLES_LC;
        const uint32 nthreads = NTHREADS_LC;

        for(uint32 j = 0; j < ncycles; ++j)
        {
            std::thread threads[nthreads];
            Basic<double> basic(nthreads);

            // have each thread run function
            for(uint32 i = 0; i < nthreads; ++i)
                threads[i] = std::move(std::thread(increment_float_with_lock,
                                 (void*) &basic));

            // ensure above is done before checking
            for(uint32 i = 0; i < nthreads; ++i)
                threads[i].join();

            CHECK_CLOSE(nthreads, basic.value, 1.0e-12);
        }
    }
    //------------------------------------------------------------------------//
    TEST(Check_Atomic_float_without_lock)
    {
        uint32 ncycles = NCYCLES_LC;
        const uint32 nthreads = NTHREADS_LC;

        for(uint32 j = 0; j < ncycles; ++j)
        {
            std::thread threads[nthreads];
            Basic<atomic<double> > basic(nthreads);

            // have each thread run function
            for(uint32 i = 0; i < nthreads; ++i)
                threads[i] = std::move(std::thread(increment_atomic_float,
                                 (void*) &basic));

            // ensure above is done before checking
            for(uint32 i = 0; i < nthreads; ++i)
                threads[i].join();

            CHECK_CLOSE(nthreads, basic.value, 1.0e-12);
        }
    }
    //------------------------------------------------------------------------//

}
