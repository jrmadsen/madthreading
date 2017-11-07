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


#include <UnitTest++/UnitTest++.h>

#include <iomanip>
#include <iostream>

#include "madthreading/atomics/atomic.hh"
#include "madthreading/atomics/atomic_array.hh"
#include "madthreading/atomics/atomic_deque.hh"
#include "madthreading/atomics/atomic_map.hh"
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
    using mad::atomic_array;
    using mad::atomic_deque;
    using mad::atomic_map;
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

    //========================================================================//
    //
    //
    //      TESTS WITH CONTAINERS
    //
    //
    //========================================================================//

#define ATOMIC_ARRAY_SIZE 4
#define NTHREADS_ARRAYS 16
    //========================================================================//
    // Below class is used to create situation where all threads < N-1 (N = num
    // of threads) wait until the last thread is spawned before proceeding to
    // the operation. This way, the threads are forced into a data race
    // and we can actually test whether the atomics/mutexes are working.
    // Otherwise, the thread executes the simple operation before the following
    // thread is spawned
    class SetupContainer
    {
    public:
        SetupContainer() : wait_until(0), counter(0) { }
        SetupContainer(const uint32& _wait) : wait_until(_wait), counter(0) { }
#ifdef DEBUG
        bool wait() const { return (counter < wait_until) ? true : false; }
#else
        bool wait() const { return false; }
#endif
        void SetWait(const uint32& val) { wait_until = val; }
        uint32 operator++() { return ++counter;  }
        uint32 operator++(int) { uint32 c = counter; counter++; return c; }

    protected:
        uint32 wait_until;
        atomic<uint32> counter;
    };
    //========================================================================//
    // Because raw threads are *easier* to implement (e.g. no package
    // dependencies)
    //========================================================================//
    class SetupIntDeque : public SetupContainer
    {
    public:
        typedef atomic_deque<uint32> Array_t;
    public:
        SetupIntDeque(const uint32& _size)
        :array(_size-1, 0) { atomic<uint32> init(0); array.resize(_size, init); }
        SetupIntDeque& operator()(uint32 i)
        {
            ++(*this);
            while(wait()) { }
            array[i] += 1;
            return *this;
        }

        const Array_t& get() const { return array; }

    protected:
        Array_t array;
    };
    //------------------------------------------------------------------------//
    struct SetupIntDequeArgs
    {
        SetupIntDequeArgs(SetupIntDeque& _setup) : setup(_setup), index(0) { }
        SetupIntDeque& setup;
        uint32 index;
    };
    //------------------------------------------------------------------------//
    inline void* SetupIntDequeProxy(void* threadarg)
    {
        struct SetupIntDequeArgs* setup_deque_args
        = (SetupIntDequeArgs*)threadarg;
        setup_deque_args->setup(setup_deque_args->index);
        return (void*) setup_deque_args;
    }
    //========================================================================//
    //========================================================================//
    class SetupIntArray : public SetupContainer
    {
    public:
        typedef atomic_array<uint32, ATOMIC_ARRAY_SIZE> Array_t;

    public:
        SetupIntArray() { }
        SetupIntArray& operator()(uint32 i)
        {
            ++(*this);
            while(wait()) { }
            array[i] += 1;
            return *this;
        }
        const Array_t& get() const { return array; }

    protected:
        Array_t array;
    };
    //------------------------------------------------------------------------//
    struct SetupIntArrayArgs
    {
        SetupIntArrayArgs(SetupIntArray& _setup) : setup(_setup), index(0) { }
        SetupIntArray& setup;
        uint32 index;
    };
    //------------------------------------------------------------------------//
    inline void* SetupIntArrayProxy(void* threadarg)
    {
        struct SetupIntArrayArgs* setup_array_args = (SetupIntArrayArgs*)threadarg;
        setup_array_args->setup(setup_array_args->index);
        return (void*) setup_array_args;
    }
    //========================================================================//
    class SetupIntMap : public SetupContainer
    {
    public:
        typedef atomic_map<uint32, uint32> Array_t;
        typedef typename Array_t::const_iterator const_iterator;

    public:
        SetupIntMap() { }
        SetupIntMap& operator()(uint32 i)
        {
            ++(*this);
            while(wait()) { }
            map[i] += 1;
            return *this;
        }
        const Array_t& get() const { return map; }

    protected:
        Array_t map;
    };
    //------------------------------------------------------------------------//
    struct SetupIntMapArgs
    {
        SetupIntMapArgs(SetupIntMap& _setup) : setup(_setup), index(0) { }
        SetupIntMap& setup;
        uint32 index;
    };
    //------------------------------------------------------------------------//
    inline void* SetupIntMapProxy(void* threadarg)
    {
        struct SetupIntMapArgs* setup_map_args
        = (SetupIntMapArgs*)threadarg;
        setup_map_args->setup(setup_map_args->index);
        return (void*) setup_map_args;
    }
    //========================================================================//
    class SetupIntMutexedPOD : public SetupContainer
    {
    public:
        typedef std::vector<atomic<uint32> > Array_t;

    public:
        SetupIntMutexedPOD(uint32 _size)
        : array(_size, 0) { }
        SetupIntMutexedPOD& operator()(uint32 i)
        {
            ++(*this);
            while(wait()) { }
            array[i] += 1;
            return *this;
        }
        const Array_t& get() const { return array; }

    protected:
        Array_t array;
    };
    //------------------------------------------------------------------------//
    struct SetupIntMutexedPODArgs
    {
        SetupIntMutexedPODArgs(SetupIntMutexedPOD& _setup)
        : setup(_setup), index(0) { }
        SetupIntMutexedPOD& setup;
        uint32 index;
    };
    //------------------------------------------------------------------------//
    inline void* SetupIntMutexedPODProxy(void* threadarg)
    {
        struct SetupIntMutexedPODArgs* setup_mpod_args
        = (SetupIntMutexedPODArgs*)threadarg;
        setup_mpod_args->setup(setup_mpod_args->index);
        return (void*) setup_mpod_args;
    }
    //========================================================================//

    //------------------------------------------------------------------------//
    TEST(Atomic_int_deque_test)
    {
        const uint32 nthreads = NTHREADS_ARRAYS;
        SetupIntDeque setup(ATOMIC_ARRAY_SIZE);
        std::thread threads[nthreads];
        uint32 index = 0;
        SetupIntDequeArgs args(setup);
        args.index = index;
        for(uint32 j = 0; j < nthreads; ++j)
            threads[j] = std::move(std::thread(SetupIntDequeProxy, (void*)&args));
        for(uint32 j = 0; j < nthreads; ++j)
            threads[j].join();
        for(uint32 j = 0; j < setup.get().size(); ++j)
        {
            if(j == index)
                CHECK_EQUAL(nthreads, setup.get()[j]);
            else
                CHECK_EQUAL(0U, setup.get()[j]);
        }
    }
    //------------------------------------------------------------------------//
    TEST(Atomic_int_array_test)
    {
        const uint32 nthreads = NTHREADS_ARRAYS;
        SetupIntArray setup;
        setup.SetWait(nthreads);
        std::thread threads[nthreads];
        uint32 index = 0;
        SetupIntArrayArgs args(setup);
        args.index = index;
        for(uint32 j = 0; j < nthreads; ++j)
            threads[j] = std::move(std::thread(SetupIntArrayProxy, (void*)&args));
        for(uint32 j = 0; j < nthreads; ++j)
            threads[j].join();
        for(uint32 j = 0; j < setup.get().size(); ++j)
        {
            if(j == index)
                CHECK_EQUAL(nthreads, setup.get()[j]);
            else
                CHECK_EQUAL(0U, setup.get()[j]);
        }
    }
    //------------------------------------------------------------------------//
    TEST(Mutexed_int_POD_test)
    {
        const uint32 nthreads = NTHREADS_ARRAYS;
        SetupIntMutexedPOD setup(ATOMIC_ARRAY_SIZE);
        setup.SetWait(nthreads);
        std::thread threads[nthreads];
        uint32 index = 0;
        SetupIntMutexedPODArgs args(setup);
        args.index = index;
        for(uint32 j = 0; j < nthreads; ++j)
            threads[j] = std::move(std::thread(SetupIntMutexedPODProxy, (void*)&args));
        for(uint32 j = 0; j < nthreads; ++j)
            threads[j].join();
        for(uint32 j = 0; j < setup.get().size(); ++j)
        {
            if(j == index)
                CHECK_EQUAL(nthreads, setup.get()[j]);
            else
                CHECK_EQUAL(0U, setup.get()[j]);
        }
    }
    //------------------------------------------------------------------------//
}
