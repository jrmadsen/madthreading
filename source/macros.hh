//
//
//
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
//
//
// created by jmadsen on Tue Jul 18 00:33:43 2017
//
//
//
//

#ifndef macros_hh_
#define macros_hh_

// Define C++0x
#ifndef CXX0X
#   if defined(__GXX_EXPERIMENTAL_CXX0X)    // C++0x
#       define CXX0X
#   endif
#endif

// Define C++11
#ifndef CXX11
#   if __cplusplus > 199711L   // C++11
#       define CXX11
#   endif
#endif

// Define C++14
#ifndef CXX14
#   if __cplusplus > 201103L   // C++14
#       define CXX14
#   endif
#endif

// Define C++17
#ifndef CXX17
#   if __cplusplus > 201402L    // C++17
#       define CXX17
#   endif
#endif

#if defined(MAD_USE_CXX98)
#   if defined(MAD_USE_CXX0X)
#       undef MAD_USE_CXX0X
#   endif

#   if defined(MAD_USE_CXX11)
#       undef MAD_USE_CXX11
#   endif

#   if defined(MAD_USE_CXX14)
#       undef MAD_USE_CXX14
#   endif

#   if defined(MAD_USE_CXX17)
#       undef MAD_USE_CXX17
#   endif
#endif

#if defined(MAD_USE_CXX0X)
#   if defined(MAD_USE_CXX11)
#       undef MAD_USE_CXX11
#   endif

#   if defined(MAD_USE_CXX14)
#       undef MAD_USE_CXX14
#   endif

#   if defined(MAD_USE_CXX17)
#       undef MAD_USE_CXX17
#   endif
#endif

#if defined(MAD_USE_CXX11)
#   if defined(MAD_USE_CXX14)
#       undef MAD_USE_CXX14
#   endif

#   if defined(MAD_USE_CXX17)
#       undef MAD_USE_CXX17
#   endif
#endif

#if defined(MAD_USE_CXX14)
#   if defined(MAD_USE_CXX17)
#       undef MAD_USE_CXX17
#   endif
#endif

#endif
