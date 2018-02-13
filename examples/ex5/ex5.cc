//
//
//	This example shows how to use task groups
//      - A correct answer will follow the order of operations:
//              (PI + PI) * PI
//      - An incorrect answer will not follow the order of operations:
//              PI + PI*PI
//
//


#ifdef USE_OPENMP
    #include <omp.h>
#endif

#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

#include <madthreading/types.hh>
#include <timemory/timer.hpp>
#include <madthreading/threading/thread_manager.hh>
#include <madthreading/allocator/allocator.hh>
#include <madthreading/utility/constants.hh>
#include <madthreading/threading/auto_lock.hh>

using namespace std;
using namespace mad;

#include "../Common.hh"

//============================================================================//

int main(int, char**)
{
    ulong_type num_threads = thread_manager::GetEnvNumThreads(1);
    thread_manager* tm = new thread_manager(num_threads);
    ulong_type nchar = 10;

    string sentence = "";
    static mad::mutex mtx;
    string soln = "__________|__________|__________|__________|__________|__________|__________|__________|__________|__________|";

    //------------------------------------------------------------------------//
    auto block_a = [&sentence] ()
    {
        auto_lock l(mtx);
        sentence += "|";
    };
    //------------------------------------------------------------------------//
    auto block_b = [&sentence] ()
    {
#ifndef __INTEL_COMPILER
        this_thread::sleep_for(chrono::milliseconds(1));
#endif
        auto_lock l(mtx);
        sentence += "_";
    };
    //------------------------------------------------------------------------//

    //========================================================================//
    // just executabe all the blocks
    // note: thread sleep in block_b will help ensure the resulting sentence
    //  is out of order
    mad::task_group<void> tg1;
    for(ulong_type j = 0; j < nchar; ++j)
    {
        for(ulong_type i = 0; i < nchar; ++i)
            tm->exec(&tg1, block_b);
        tm->exec(&tg1, block_a);
    }
    tg1.join();
    //========================================================================//

    tmcout << "Sentence A: " << sentence << endl;
    string sentA = sentence;
    sentence = "";

    //========================================================================//
    // new task group is created implicitly
    mad::task_group<void> tg2a;
    for(ulong_type j = 0; j < nchar; ++j)
    {
        mad::task_group<void> tg2b;
        for(ulong_type i = 0; i < nchar; ++i)
            tm->exec(&tg2b, block_b);
        // here we want thread_group 2b to finish
        tg2b.join();
        tm->exec(&tg2a, block_a);
    }
    tg2a.join();
    //========================================================================//

    tmcout << "Sentence B: " << sentence << endl;
    delete tm;
    return (soln != sentence && sentA == sentence);
}

//============================================================================//

