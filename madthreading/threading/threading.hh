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
// created by jrmadsen on Sun Jun 21 17:28:55 2015
//
//
//
//


#ifndef threading_hh_
#define threading_hh_

#include <sstream>
#include <exception>
#include <stdexcept>
#include <thread>
#include <mutex>

//----------------------------------------------------------------------------//

namespace mad
{

    using thread = std::thread;

    typedef void* ThreadFuncReturnType;
    typedef void* ThreadFuncArgType;

    typedef std::thread::id Pid_t;

} // namespace mad


#include "tls.hh"

//============================================================================//
// Some functions that help with threading
namespace mad
{

namespace Threading
{

//----------------------------------------------------------------------------//

mad::Pid_t GetPidId();
int GetNumberOfCores();

//----------------------------------------------------------------------------//

} // namespace pthreading

} // namespace mad

//============================================================================//

#endif
