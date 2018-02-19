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
#include <madthreading/threading/task/task_group.hh>

//----------------------------------------------------------------------------//

namespace py = pybind11;

typedef mad::details::vtask             vtask;
typedef mad::details::vtask_group       vtask_group;
typedef vtask_group::task_count_type    task_count_type;
typedef mad::task_group<void>           void_task_group;

typedef const task_count_type& (vtask_group::*task_count_func_type)() const;
typedef const mad::ulong_t& (vtask_group::*id_func_type)() const;
typedef void (std::promise<int64_t>::*int64_promise_func_type)(const int64_t&);
typedef void (void_task_group::*join_void_func)();

PYBIND11_MODULE(pytask, t)
{
    py::class_<void_task_group> task_group(t, "task_group");
    task_group.def(py::init<>())
              .def("join",
                   (join_void_func) &void_task_group::join,
                   "Join function")
              .def("task_count",
                   (task_count_func_type) &vtask_group::task_count,
                   "Get the task count")
              .def("id",
                   (id_func_type) &vtask_group::id,
                   "Get the ID");

    py::class_<std::future<int64_t>> fint (t, "int64_future");
    fint.def("get", &std::future<int64_t>::get, "Return the result")
        .def("wait", &std::future<int64_t>::wait,
             "Wait for the result to become avail");

    py::class_<std::promise<int64_t>> fprom (t, "int64_promise");
    fprom.def("get_future", &std::promise<int64_t>::get_future,
              "returns a future associated with the promised result")
         .def("set_value",
              (int64_promise_func_type) &std::promise<int64_t>::set_value,
              "sets the result to specific value");
}

//----------------------------------------------------------------------------//
