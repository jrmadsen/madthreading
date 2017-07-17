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
// created by jmadsen on Sat Jul 15 18:43:49 2017
//
//
//
//


#ifndef task_group_version_hh_
#define task_group_version_hh_

//----------------------------------------------------------------------------//
#ifdef SWIG
%module task_group
%{
    #define SWIG_FILE_WITH_INIT
    #include "task_group.hh"
%}

%include "task_group.hh"
#endif
//----------------------------------------------------------------------------//

// Define C++11
#ifndef CXX11
#   if __cplusplus > 199711L   // C++11
#       define CXX11
#   endif
#endif

#if defined(CXX11)
#	include "cxx11-compat/task_group.hh"
#else
#	include "cxx98-compat/task_group.hh"
#endif

#endif

