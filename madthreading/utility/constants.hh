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
// created by jmadsen on Sat Jul 15 16:10:46 2017
//
//
//
//

#ifndef constants_hh_
#define constants_hh_


//============================================================================//

namespace mad
{

namespace dat
{

//----------------------------------------------------------------------------//
// General Constants

// Static String Lengths
static size_t const STRLEN = 256;
static size_t const BIGSTRLEN = 4096;

// PI
static double const PI = 3.14159265358979323846;

// PI/2
static double const PI_2 = 1.57079632679489661923;

// PI/4
static double const PI_4 = 0.78539816339744830962;

// 1/PI
static double const INV_PI = 0.31830988618379067154;

// 1/(2*PI)
static double const INV_TWOPI = 0.15915494309189533577;

// 2/PI
static double const TWOINVPI = 0.63661977236758134308;

// 2/3
static double const TWOTHIRDS = 0.66666666666666666667;

// 2*PI
static double const TWOPI = 6.28318530717958647693;

// 1/sqrt(2)
static double const INVSQRTTWO = 0.70710678118654752440;

// tan(PI/12)
static double const TANTWELFTHPI = 0.26794919243112270647;

// tan(PI/6)
static double const TANSIXTHPI = 0.57735026918962576451;

// PI/6
static double const SIXTHPI = 0.52359877559829887308;

// 3*PI/2
static double const THREEPI_2 = 4.71238898038468985769;

// Degrees to Radians
static double const DEG2RAD = 1.74532925199432957692e-2;

//----------------------------------------------------------------------------//

} // namespace data

} // namespace mad

//----------------------------------------------------------------------------//

#endif
