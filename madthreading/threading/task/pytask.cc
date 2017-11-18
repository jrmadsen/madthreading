// MIT License
// Copyright (c) 2017 Jonathan R. Madsen
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

#include <functional>
#include <future>

#include <madthreading/threading/task/task.hh>
#include <madthreading/threading/task/task_tree.hh>
#include <madthreading/threading/task/task_group.hh>

//----------------------------------------------------------------------------//

namespace py = pybind11;

typedef mad::task_group::task_count_type task_count_type;

typedef const task_count_type& (mad::task_group::*task_count_func_type)() const;
typedef const mad::ulong_type& (mad::task_group::*id_func_type)() const;
typedef void (std::promise<int>::*int_promise_func_type)(const int&);
typedef void (mad::task_group::*join_void_func)();

PYBIND11_MODULE(pytask, t)
{
    py::class_<mad::task_group> task_group(t, "task_group");
    task_group.def(py::init<>())
              .def("join",
                   (join_void_func) &mad::task_group::join,
                   "Join function")
              .def("task_count",
                   (task_count_func_type) &mad::task_group::task_count,
                   "Get the task count")
              .def("id",
                   (id_func_type) &mad::task_group::id,
                   "Get the ID");

    py::class_<std::future<int>> fint (t, "int_future");
    fint.def("get", &std::future<int>::get, "Return the result")
        .def("wait", &std::future<int>::wait,
             "Wait for the result to become avail");

    py::class_<std::promise<int>> fprom (t, "int_promise");
    fprom.def("get_future", &std::promise<int>::get_future,
              "returns a future associated with the promised result")
         .def("set_value",
              (int_promise_func_type) &std::promise<int>::set_value,
              "sets the result to specific value");
}

//----------------------------------------------------------------------------//
