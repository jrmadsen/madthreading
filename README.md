# madthreading
A low-overhead, task-based threading API using a pool of C++11 threads (i.e. thread pool)

Madthreading is a general multithreading API similar to Intel's Thread Building
Blocks but with a more flexible tasking system. For example, in TBB, task
groups cannot return and the function pointer does not support arguments. 
These features are available here. Additionally, there is a bit more transparency
and simplicity to the inner workings and the overhead has been reduced. However,
advanced features like flowgraph are not available. Is it better than TBB? I am
not sure but I certainly like it better than OpenMP

Side note about OpenMP:
  - There is a common misconception that all you need to do to multithread with OpenMP is add a `#pragma omp parallel`... 
  - This is very misleading, although it will generally work, the performance is usually terrible because of things like false-sharing
  - To fix the false-sharing and improve performance, the same amount of coding work usually need to be done as when dealing with raw threads or TBB
    - At this point, you are doing the same amount of work but add the following issues:
      - (1) OpenMP is only supported by certain compilers
      - (2) OpenMP injects "ghost" code that makes performance tuning and debugging difficult because you never see the code
      - (3) weird things like using a lambda (which is inlined) not having the same performance as not using a lambda, even though it absolutely should be
        - I actually showed this to OpenMP developers and they did not know why because of #2 but confirmed there shouldn't be a performance hit
      - (4) bugs like #3 are compiler-specific and therefore performance isn't as portable
      - (5) Pragmas make your code look ugly
  - Example #6 (examples/ex6) demonstrates many of the different implementations for multithreading
  
Features:
  - Low overhead
  - Work-stealing (inactive thread grabs work off stack of tasks)
  - Thread-pool (no overhead of thread creation)
  - Interface takes any function construct
  - Support for return types from joining (e.g. summation from all threads)
  - Background tasks via pointer signaling
    
The primary benefit of using Madthreading is the creation of a
thread-pool. Threads are put to sleep when not doing work and do not require
compute cycles when not in use.

The thread-pool is created during the instantiation of the
thread-manager. Once the thread-manager has been created, you simply
pass functions with or without arguments to the thread-manager, which
creates tasks and these tasks are iterated over until the task stack is
empty.

Passing tasks to thread-manager is done through three primary interfaces:

```c++
 mad::thread_manager::exec(mad::task_group*, ...)       // pass one task to the stack
 mad::thread_manager::run(mad::task_group*, ...)        // similar to exec but N times
                                                        // where N = mad::thread_manager::size()
 mad::thread_manager::run_loop(mad::task_group*, ...)   // generic construct
```

Tasks are not explicitly created. However, you are required to pass a pointer
to a task-group. The task_group is the handle for joining/synchronization.
Instead of explicitly creating tasks, you pass function pointers and
arguments, where the arguments are for the function (exec, run) or for the
loop that creates the tasks (run_loop). Examples are provided in the
examples/ directory of the source code

Currently, there is support for functions using an unlimited number of arguments.

The number of threads is controlled via the environment variable FORCE_NUM_THREADS
or MAD_NUM_THREADS, with the latter taking supremacy.

Required dependencies:
  - GNU, Clang, or Intel compiler supporting C++11
  - CMake
   
Optional dependencies:
  - UnitTest++ (for unit testing)
    - sudo apt-get install libunittest++-dev (Ubuntu)
  - TBB (used for allocators and in examples)
  - OpenMP (used for SIMD and in examples)
  - SWIG version 3.0+ (some support for Python wrapping)

Madthreading provides a generic interface to using atomics and, when C++11 is
not available, provides a mutexed-based interface that works like an atomic.

There are two forms of tasks: standard and a tree type design. The tree
type design is intended to be closer to the TBB design.

 ##################################################

Examples:
  - ex1  : simple usage examples
  - ex2  : simple MT pi calculation using run_loop
  - ex3a : simple MT pi calculation using task_tree
  - ex3b : simple MT pi calculation using task_tree + grainsize
  - ex4  : a vectorization example using intrinsics
  - ex5  : demonstration of task_group usage cases
  - ex6  : PI calculations using different threading methods (for comparison)
    - serial
    - TBB (CXX98-body)
    - TBB (CXX11-lambda),
    - pure pthreads
    - C++11 threads
    - OpenMP (loop + reduction)
    - OpenMP (task)
    - OpenMP (parallel block - type 1)
    - OpenMP (parallel block - type 2)
    - mad thread-pool (run_loop)
    - mad thread-pool (task_tree)
    - mad thread-pool (task_tree w/ grainsize)

 ##################################################
    
Examples of OpenMP issues:

- On GCC 4.8.2 (possibly fixed, I showed this to a developer during a tutorial)
  - Here is the code he gave me (included in examples/ex6/omp_pi_loop.cc). This code utilized ~400% of the CPUs on 4 threads (on a 4 core machine), in other words, perfect speed-up:


```c++
#pragma omp parallel
{
    #pragma omp for reduction(+:sum)
    for(ulong_type i = 0; i < num_steps; ++i)
    {
        double_type x = (i-0.5)*step;
        sum = sum + 4.0/(1.0+x*x);
    }
}
```

  - I made one change, because I like to try things, which was completely valid for the C++ standard (C++11), which was the use of lambda (which would be inlined):

```c++
#pragma omp parallel
{
    auto calc_x = [step] (ulong_type i) { return (i-0.5)*step; };
    #pragma omp for reduction(+:sum)
    for(ulong_type i = 0; i < num_steps; ++i)
    {
        double_type x = calc_x(i);
        sum = sum + 4.0/(1.0+x*x);
    }
}
```

  - My CPU utilization dropped down from ~400% to ~250%. I was shocked. 
  - My first thought wasn't that it was OpenMP's fault but mine. 
  - I tried different captures, different placements of the lambda (inside and outside parallel section, inside the loop, etc.) and none of that fixed the performance. 
  - Then I thought, maybe since GCC just implemented C++11 in 4.7, the problem might be on the GCC side.
  - I tested the comparison with TBB and found no difference. 
  - So I brought it to the OpenMP developer and showed him and he verified everything and tried fixing the performance himself to no avail. 
  - He wrote down the compiler version and said he'd look into it. 


This experience started me down the road to the opinion I have of OpenMP today:

- **OpenMP is convenient but far too opaque in what is being done "under the hood" to allow straight-forward diagnosis of performance issues.**
- **False-sharing is very easy to introduce, requires a lot of experience to quickly diagnose, and is a byproduct of the OpenMP pragma style. It is much less common with other models because of how you are forced to build the code (functionally).**
- **Thus, you could end up spending far too much of your own time either (a) trying to fix something that shouldn't have to be fixed, (b) searching for the performance bottleneck that would exist in other threading models, or (c) both.**



- On GCC 5.4.1 with a very large amount of data. It took 118 seconds at 8% CPU
utilization with two threads (max = 200%).

```c++
// > [cxx] ctoast_cov_accumulate_zmap
// : 118.837 wall,   4.820 user +   4.650 system =   9.470 CPU [seconds] (  8.0%)
// (total # of laps: 32)
#pragma omp parallel default(shared)
{
    int64_t i, j, k;
    int64_t hpx;
    int64_t zpx;

    int threads = 1;
    int trank = 0;

    #ifdef _OPENMP
    threads = omp_get_num_threads();
    trank = omp_get_thread_num();
    #endif

    int tpix;

    for (i = 0; i < nsamp; ++i) 
    {
        if ((indx_submap[i] >= 0) && (indx_pix[i] >= 0)) 
        {
            hpx = (indx_submap[i] * subsize) + indx_pix[i];
            tpix = hpx % threads;
            if ( tpix == trank ) 
            {
                zpx = (indx_submap[i] * subsize * nnz) + (indx_pix[i] * nnz);

                for ( j = 0; j < nnz; ++j )
                    zdata[zpx + j] += scale * signal[i] * weights[i * nnz + j];
            }
        }
    }
}
```

The fix is below. It ran in 1.474 seconds at 200% CPU utilization. It was 
amazingly simple...

```c++
// > [cxx] accumulate_zmap_direct
// :   1.474 wall,   2.930 user +   0.020 system =   2.950 CPU [seconds] (200.1%)
// (total # of laps: 32)
#pragma omp parallel default(shared)
{
    int threads = 1;
    int trank = 0;

#ifdef _OPENMP
    threads = omp_get_num_threads();
    trank = omp_get_thread_num();
#endif

    for (int64_t i = 0; i < nsamp; ++i )
    {
        if ((indx_submap[i] >= 0) && (indx_pix[i] >= 0))
        {
            int64_t hpx = (indx_submap[i] * subsize) + indx_pix[i];
            int64_t tpix = hpx % threads;
            if ( tpix == trank )
            {
                int64_t zpx = (indx_submap[i] * subsize * nnz)
                              + (indx_pix[i] * nnz);

                for (int64_t j = 0; j < nnz; ++j )
                    zdata[zpx + j] += scale * signal[i] * weights[i * nnz + j];
            }
        }
    }
}
```

Notice the difference? "i", "j", "k", "hpx", "tpix", and "zpx" are locally declared instead of at the start of the parallel block. How did this make such a big difference? I HAVE NO IDEA BECAUSE I COULDN'T SEE WHAT OPENMP WAS DOING!

