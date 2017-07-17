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
#include <madthreading/utility/timer.hh>
#include <madthreading/threading/thread_manager.hh>
#include <madthreading/vectorization/vectorization_typedefs.hh>
#include <madthreading/allocator/allocator.hh>
#include <madthreading/utility/constants.hh>
#include <madthreading/threading/auto_lock.hh>

using namespace std;
using namespace mad;

#include "../Common.hh"

//============================================================================//

int main(int argc, char** argv)
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
        this_thread::sleep_for(chrono::milliseconds(1));
        auto_lock l(mtx);
        sentence += "_";
    };
    //------------------------------------------------------------------------//

    //========================================================================//
    // just executabe all the blocks
    // note: thread sleep in block_b will help ensure the resulting sentence
    //  is out of order
    for(ulong_type j = 0; j < nchar; ++j)
    {
        for(ulong_type i = 0; i < nchar; ++i)
            tm->exec(block_b);
        tm->exec(block_a);
    }
    tm->join();
    //========================================================================//

    tmcout << "Sentence A: " << sentence << endl;
    string sentA = sentence;
    sentence = "";

    //========================================================================//
    // new task group is created implicitly
    thread_manager* tm_tg = tm->clone(); // clone can take specific task_group
    for(ulong_type j = 0; j < nchar; ++j)
    {
        for(ulong_type i = 0; i < nchar; ++i)
            tm_tg->exec(block_b);
        // here we want tm_tg finish
        tm_tg->join();
        // note: in this example, we could accomplish the same results by
        //      not using/creating "tm_tg" and just calling "tm->join()"
        //      but that is not the purpose of this example
        tm->exec(block_a);
    }
    tm->join();
    //========================================================================//

    tmcout << "Sentence B: " << sentence << endl;

    delete tm_tg;
    delete tm;
    return (soln != sentence && sentA == sentence);
}

//============================================================================//

