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

//  VERSION_STRING 0_1
//  madthreading version.hh configuration header file  ---------------------//

//  (C) Copyright Jonathan Madsen 2015.

#ifndef madthreading_VERSION_HH_
#define madthreading_VERSION_HH_

//
//  Caution, this is the only madthreading header that is guarenteed
//  to change with every release, including this header
//  will cause a recompile every time a new madthreading version is
//  released.
//
//  madthreading_VERSION % 100 is the patch level
//  madthreading_VERSION / 100 % 1000 is the minor version
//  madthreading_VERSION / 100000 is the major version

#define madthreading_VERSION 000100
#define madthreading_MAJOR_VERSION 0
#define madthreading_MINOR_VERSION 1
#define madthreading_PATCH_VERSION 0

//
//  madthreading_LIB_VERSION must be defined to be the same as
//  madthreading_VERSION but as a *string* in the form "x_y[_z]" where x is
//  the major version number, y is the minor version number, and z is the patch
//  level if not 0.

#define madthreading_LIB_VERSION "0_1"

#endif
