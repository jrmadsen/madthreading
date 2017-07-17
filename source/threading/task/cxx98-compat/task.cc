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
// created by jrmadsen on Wed Jul 22 09:15:04 2015
//
//
//
//


#include "task.hh"
#include "task_group.hh"
#include "thread_manager.hh"
#include <cassert>

namespace mad
{

//============================================================================//

vtask::vtask(task_group* tg, void* result, void* arg1, void* arg2, void* arg3)
: m_group(tg),
  m_force_delete(false),
  m_is_stored_elsewhere(false),
  m_result(result)
{
    _check_group();
    set_arg_array(arg1, arg2, arg3);
}

//============================================================================//

void vtask::_check_group()
{
    if(!m_group)
        throw std::runtime_error("Null pointer to task_group");
    //tmcout << "Task " << this << " has task group : " << m_group
    //       << " (id = " << m_group->id() << ")" << std::endl;
}

//============================================================================//

} // namespace mad
