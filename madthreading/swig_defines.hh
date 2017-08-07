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
// created by jmadsen on Fri Jul 22 10:24:35 2016
//
//
//
//

#ifndef swig_defines_hh_
#define swig_defines_hh_

#ifdef SWIG
//#pragma SWIG nowarn=401
//#pragma SWIG nowarn=501
//#pragma SWIG nowarn=503
//#pragma SWIG nowarn=505

%include "std_vector.i"
%include "std_string.i"
%include "std_deque.i"
%include "std_map.i"

%import "std_alloc.i"
%import "std_array.i"
%import "std_basic_string.i"
%import "std_char_traits.i"
%import "std_common.i"
%import "std_container.i"
%import "std_except.i"
%import "std_ios.i"
%import "std_iostream.i"
%import "std_list.i"
%import "std_multimap.i"
%import "std_multiset.i"
%import "std_pair.i"
%import "std_set.i"
%import "std_sstream.i"
%import "std_streambuf.i"
%import "std_unordered_map.i"
%import "std_unordered_multimap.i"
%import "std_unordered_multiset.i"
%import "std_unordered_set.i"
%import "std_vectora.i"
%import "std_wios.i"
%import "std_wiostream.i"
%import "std_wsstream.i"
%import "std_wstreambuf.i"
%import "std_wstring.i"

// Instantiate templates
namespace std
{
    %template(IntVector) vector<int>;
    %template(DoubleVector) vector<double>;
    %template(StringVector) vector<string>;
    %template(ConstCharVector) vector<const char*>;

    %template(IntDeque) deque<int>;
    %template(DoubleDeque) deque<double>;
    %template(StringDeque) deque<string>;
    %template(ConstCharDeque) deque<const char*>;
}

%include "swig_defines.hh"
#endif


//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//

#endif
