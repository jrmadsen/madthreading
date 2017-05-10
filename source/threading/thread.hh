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

//
//
//
//
//
// created by jrmadsen on Wed Jul 22 10:52:52 2015
//
//
//
//

#ifndef thread_hh_
#define thread_hh_

#include "atomic.hh"
#include "AutoLock.hh"
#include "allocator.hh"

#include <iostream>
#include <iomanip>
#include <sstream>

namespace threading
{
    //========================================================================//

    enum ThreadStatus
    {
        UNINITIALIZED = 0,
        START = 1,
        RUN = 2,
        RUN_ONCE = 3,
        SLEEPING = 4
    };

    //========================================================================//

    template <typename _Func>
    struct CountingThread
    {
        typedef size_t size_type;
        typedef _Func function_type;

        CountingThread()
        : count_id(thread_count++),
          core_thread(new CoreThread),
          _status(static_cast<unsigned>(UNINITIALIZED))
        { }
        CountingThread(function_type _func)
        : count_id(thread_count++),
          core_thread(new CoreThread),
          _status(static_cast<unsigned>(UNINITIALIZED))
        {
            static CoreMutex mutex = CORE_MUTEX_INITIALIZER;
            AutoLock lock(&mutex);
            SetFunction(_func);
        }

        virtual ~CountingThread()
        {
            thread_count -= 1;
            delete core_thread; core_thread = 0;
        }

        static void SetFunction(function_type _func) { function = _func; }

        static atomic<size_type> thread_count;
        size_type count_id;
        CoreThread* core_thread;
        static function_type function;

        void set(ThreadStatus _stat) { _status = static_cast<unsigned>(_stat); }
        ThreadStatus status() const { return static_cast<ThreadStatus>(atomics::get<unsigned>(_status)); }

    private:
        typedef atomic<unsigned> ThreadStatus_t;
        ThreadStatus_t _status;
    };

    //========================================================================//

    template <typename _Func>
    atomic<typename CountingThread<_Func>::size_type>
    CountingThread<_Func>::thread_count
    = atomic<CountingThread<_Func>::size_type>(0);

    template <typename _Func>
    typename CountingThread<_Func>::function_type
    CountingThread<_Func>::function = 0;

    //========================================================================//
    // pre-declaration
    template <typename _Args, typename _Func>
    struct Thread;

    //========================================================================//

    template <typename _Args, typename _Func>
    struct ThreadSpinner
    {
        typedef Thread<_Args, _Func> thread_type;
        typedef bool Bool_t;
        //typedef atomic<unsigned short> Bool_t;

        thread_type* this_thread;
        Bool_t spin;
        Bool_t alive;
        Bool_t finished;

        ThreadSpinner()
        : this_thread(0), spin(false), alive(true), finished(false)
        { }
        ThreadSpinner(thread_type* _t)
        : this_thread(_t), spin(false), alive(true), finished(false)
        { }
        virtual ~ThreadSpinner() { this_thread = 0; }
    };

    //========================================================================//

    template <typename _Args, typename _Func>
    struct Thread : public CountingThread<_Func>
    {
        typedef _Args argument_type;
        typedef _Func function_type;
        typedef CountingThread<function_type> Base_t;
        typedef typename Base_t::size_type size_type;
        typedef Thread<argument_type, function_type> this_type;

        Thread() : _args(0) { }
        Thread(function_type _func) : Base_t(_func), _args(0) { }
        Thread(function_type _func, argument_type* _arg)
        : Base_t(_func), _args(_arg)
        { }

        virtual ~Thread() { delete _args; _args = 0; }

        void SetArguments(argument_type* _an_arg) { _args = _an_arg; }
        argument_type* GetArguments() const { return _args; }
        void set_args(argument_type* _an_arg) { _args = _an_arg; }
        argument_type* args() const { return _args; }

        //--------------------------------------------------------------------//
        static bool run_function(argument_type* thread_arg)
        {
            if(Base_t::function)
            {
                (*Base_t::function)(thread_arg);
                return true;
            }
            return false;
        }
        //--------------------------------------------------------------------//
        static void* thread_function(void* _thread_arg)
        {
            func_argument_type* thread_arg = (func_argument_type*)(_thread_arg);

            if(!thread_arg)
                return _thread_arg;

            if(thread_arg->second->this_thread->status() == RUN_ONCE)
            {
                this_type::run_function(thread_arg->first);
                thread_arg->second->finished = true;
                thread_arg->second->spin = true;
            } else
            {
                while(thread_arg->second->alive)
                {
                    if(this_type::EXIT_LOOP || !thread_arg->second->alive)
                    {
                        thread_arg->second->finished = true;
                        thread_arg->second->spin = true;
                        thread_arg->second->this_thread->set(UNINITIALIZED);
                        CORETHREADEXIT(NULL);
                        break;
                    }

                    if((thread_arg->second->this_thread->status() == START ||
                        thread_arg->second->this_thread->status() == RUN))
                    {
                        if(this_type::run_function(thread_arg->first))
                        {
                            thread_arg->second->finished = true;
                            thread_arg->second->spin = true;
                            thread_arg->second->this_thread->set(SLEEPING);
                        }
                    }
                }

            }

            thread_arg->second->spin = true;
            thread_arg->second->finished = true;
            thread_arg->second->this_thread->set(UNINITIALIZED);

            CORETHREADEXIT(NULL);


        }
        //--------------------------------------------------------------------//
        argument_type* _args;
        static atomic<unsigned short> EXIT_LOOP;
        //--------------------------------------------------------------------//

    };

    //========================================================================//

    template <typename _Args, typename _Func>
    atomic<unsigned short> Thread<_Args, _Func>::EXIT_LOOP = false;

    //========================================================================//

//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//

} // namespace threading

#endif
