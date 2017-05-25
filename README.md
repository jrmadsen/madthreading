# madthreading
A low-overhead, task-based threading API using a thread-pool

Madthreading (Mad prefix is not an acronym, it is simply a shortened version
of my name) is a general multithreading API that is backwards compatible with
C++98 using pthreads -- support for Windows will likely not be written by me.

Madthreading is a work-stealing model with low overhead. A set of threads are
placed in a pool and woken when tasks are passed onto the stack.

Madthreading is an excellent option to quickly add multithreading to an
existing project, especially those who do not require compilation with C++11
support.

The primary benefit of using Madthreading is the creation of a
thread-pool. Threads are put to sleep when not doing work and do not require
compute cycles when not in use.

The thread-pool is created during the instantiation of the
thread-manager. Once the thread-manager has been created, you simply add
pass functions with or without arguments to the thread-manager, which
creates tasks and these tasks are iterated over until the task stack is
empty.

Passing tasks to thread-manager is done through three primary interfaces:

```c++
 mad::thread_manager::exec(...)       // pass one task to the stack
 mad::thread_manager::run(...)        // similar to exec but N times
                                      // where N = mad::thread_manager::size()
 mad::thread_manager::run_loop(...)   // generic construct
```

Tasks are not explicitly created. Instead, you pass function pointers and
arguments, where the arguments are for the function (exec, run) or for the
loop that creates the tasks (run_loop). Examples are provided in the
examples/ directory of the source code

Currently, there is support for functions using 3 arguments. If you compile
your code with C++11 enabled, additional arguments can easily handled
through the use of lambdas or the STL bind:

```c++
// Number of threads = 4 (default) or what environment variable
// FORCE_NUM_THREADS is set to
mad::thread_manager* tm
     = new mad::thread_manager(mad::thread_manager::GetNumThreads(4));

using namespace std::placeholders;

auto arg4 = 2;
auto arg5 = 4.5;
auto arg6 = -1;

auto func1 = [arg4, arg5, arg6] (int arg1, double arg2, long arg3)
{
     full_func(arg1, arg2, arg3, arg4, arg5, arg6);
};

auto func2 = std::bind(full_func, _1, _2, _3, arg4, arg5, arg6);

tm->exec<void>(func1, 1, 1.3, 5L);
tm->exec<void>(func2, 3, 4.1, 7L);

tm->join();
```

At some point in the future, when Madthreading is compiled with C++11,
I will use template forwarding to handle any amount of function parameters
but for the time being, simply use this easy workaround.

Madthreading provides a generic interface to using atomics and, when C++11 is
not available, provides a mutexed-based interface that works like an atomic.

Several containers (array, deque, map, vector) using atomics as their data
type are available in the package. The usage of these are not guaranteed to
be thread-safe when resizing, etc.

There are two forms of tasks: standard and a tree type design. The tree
type design is intended to be closer to the TBB design.