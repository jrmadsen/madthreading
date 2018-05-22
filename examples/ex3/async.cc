//
//  Demonstration of using asynchronous execution to run Python and C++
//  alongside each other
//
//  This model allows "scheduling" work to be done in C++ but returning to
//  Python (where more work in Python can be done) while the C++ runs in
//  the background
//
//  When the Python code needs the calculations from C++, it calls "get"
//  on the future returned to Python. If the C++ calculation finished in the
//  background, "get" just returns the result. If the C++ calculation has not
//  finished, then the "get" call blocks until the calculation has finished
//

#include <future>
#include <chrono>
#include <functional>
#include <mutex>
#include <thread>
#include <atomic>
#include <iostream>
#include <cstdint>
#include <sstream>
#include <map>
#include <string>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>

#include <timemory/auto_timer.hpp>
#include <timemory/manager.hpp>

#include <madthreading/threading/thread_manager.hh>

namespace py = pybind11;
typedef py::array_t<int64_t, py::array::c_style | py::array::forcecast> iarray_t;
constexpr int64_t fibonacci_max = 43;
constexpr int64_t cxxfib = 40;

//============================================================================//
//  Declaration of helper function
//============================================================================//
void output_message(int64_t);
void write(const std::string&);

//============================================================================//
//  Main code
//============================================================================//

// main "work" calculation
int64_t fibonacci(int64_t n)
{
    if (n < 2) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}

//----------------------------------------------------------------------------//
// calculate fibonacci to do various amounts of work
int64_t async_work_np()
{
    //TIMEMORY_AUTO_TIMER();
    // static atomic counter
    //static std::atomic<int64_t> ncall;
    // thread-local counter
    //int64_t _ncall = ncall++;
    // simple message reporting we are in C++
    //output_message(_ncall);
    // do fibonacci. Do not do with value larger than 43 since those take
    // alot of time
    return fibonacci(cxxfib);
}

//----------------------------------------------------------------------------//
// calculate fibonacci to do various amounts of work
int64_t async_work(int64_t _ncall)
{
    // simple message reporting we are in C++
    //output_message(_ncall);
    // do fibonacci. Do not do with value larger than 43 since those take
    // alot of time
    //TIMEMORY_AUTO_TIMER();
    return fibonacci(cxxfib);
}

//============================================================================//

template <typename _Tp>
class py_future : public std::shared_future<_Tp>
{
public:
    typedef std::shared_future<_Tp> future_type;

public:
    py_future() : future_type() {}
    explicit py_future(const py_future& rhs) : future_type(rhs) { }
    explicit py_future(const future_type& rhs) : future_type(rhs) { }
    explicit py_future(std::future<_Tp>& rhs) : future_type(rhs.share()) { }
};

//============================================================================//

//============================================================================//
//  Python wrappers
//============================================================================//

PYBIND11_MODULE(async, t)
{
    auto py_future_init = [=] () { return new py_future<int64_t>(); };

    // we have to wrap each return type
    py::class_<py_future<int64_t>, std::unique_ptr<py_future<int64_t>, py::nodelete>>
            (t, "int64_future_t")
        .def(py::init(py_future_init), "Initialization")
        .def("get", &py_future<int64_t>::get, "Return the result")
        .def("wait", &py_future<int64_t>::wait,
             "Wait for the result to become avail");

    //------------------------------------------------------------------------//
    // called from Python in a loop. In a standard MT situation, this would
    // normally block before returning to Python Interp or you would have to
    // move the loop down into the C++ layer for performance
    auto run_func = [&] ()
    {
        // async will use a thread pool and defer executation until a later
        // time or when the std::future it returns calls its member function
        // "get()"
        std::cout << mad::thread_manager::instance() << std::endl;
        auto f = mad::thread_manager::instance()->async<int64_t>(async_work_np);
        return new py_future<int64_t>(f);
    };
    //------------------------------------------------------------------------//
    auto work_func = [&] (int64_t n)
    {
        std::cout << mad::thread_manager::instance() << std::endl;
        auto f = mad::thread_manager::instance()->async<int64_t>(async_work, n);
        return new py_future<int64_t>(f);
    };
    //------------------------------------------------------------------------//
    auto report_func = [=] ()
    {
        std::stringstream ss;
        mad::manager::instance()->report(ss, true);
        std::cout << ss.str() << std::endl;
    };
    //------------------------------------------------------------------------//
    auto write_func = [&] (const std::string& str)
    {
        write(str);
    };
    //------------------------------------------------------------------------//

    // the function called from Python loop
    t.def("write", write_func, "Print but using std::cout")
     .def("run", run_func, "Run asynchron", py::return_value_policy::automatic_reference)
     .def("work", work_func, "run async function", py::return_value_policy::automatic_reference)
     .def("report", report_func, "report timing");
}

//============================================================================//
//  IMPLEMENTATION OF HELPER FUNCTIONS
//============================================================================//
typedef std::recursive_mutex mutex_t;
typedef std::vector<std::string> str_vector_t;
using auto_lock_t = std::lock_guard<mutex_t>;

static mutex_t os_mutex;

//----------------------------------------------------------------------------//
// delimit line
str_vector_t delimit(const std::string& line, char delimiter)
{
    str_vector_t delimitedLine;
    std::string token;
    std::istringstream iss(line);
    while( getline(iss,token,delimiter) )
        if( !token.empty() )
            delimitedLine.push_back(token);
    return delimitedLine;
}

//----------------------------------------------------------------------------//
// Function used to write
void write(const std::string& str)
{
#ifdef BLOCKING_OUTPUT
    auto_lock_t gl(os_mutex);
#endif

    auto vstr = delimit(str, ' ');
    for(const auto& itr : vstr)
        std::cout << itr << " ";
    std::cout << std::endl;
}

//----------------------------------------------------------------------------//
// Assign a numerical ID to the thread ID starting a 1
int64_t get_tid()
{
    static std::atomic<int64_t> tid;
    static std::map<std::thread::id, int64_t> tids;

    if(tids.find(std::this_thread::get_id()) != tids.end())
        return tids[std::this_thread::get_id()];

    static mutex_t _mutex;
    auto_lock_t l(_mutex);
    tids[std::this_thread::get_id()] = ++tid;
    return tids[std::this_thread::get_id()];
}

//----------------------------------------------------------------------------//
// output function
void output_message(int64_t _ncall)
{
    // this shows (via get_tid == "get thread id") that this implementation
    // is not using a thread pool... we can fix that
    int64_t tid = get_tid();

    std::stringstream ss;
    ss << "(C++) Function call #" << _ncall << " on thread # "
       << tid << " (tid: " << std::this_thread::get_id()
       << "). function: fibonacci(" << (_ncall % fibonacci_max)
       << ")...";

#ifdef BLOCKING_OUTPUT
    auto_lock_t l(os_mutex);
#endif

    write(ss.str());
}

//----------------------------------------------------------------------------//
