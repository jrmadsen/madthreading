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
    