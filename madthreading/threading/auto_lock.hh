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


#ifndef autolock_hh_
#define autolock_hh_

#ifdef SWIG

%module auto_lock
%{
    #define SWIG_FILE_WITH_INIT
    #include "madthreading/threading/threading.hh"
    #include "madthreading/threading/template_auto_lock.hh"
    #include "madthreading/threading/auto_lock.hh"
%}

%import "madthreading/threading/threading.hh"
#include "madthreading/threading/threading.hh"
%import "madthreading/threading/mutex.hh"
%include "auto_lock.hh"
#endif

#include "madthreading/threading/threading.hh"
#include "madthreading/threading/mutex.hh"

//----------------------------------------------------------------------------//

namespace mad
{

using auto_lock = std::lock_guard<std::recursive_mutex>;

} // namespace mad

//----------------------------------------------------------------------------//

#endif // autolock_hh_
