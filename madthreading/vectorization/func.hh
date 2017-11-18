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
// created by jmadsen on Sat Jul 15 17:35:09 2017
//
//
//
//

#ifndef func_hh_
#define func_hh_

//============================================================================//

namespace mad
{

namespace func
{

//----------------------------------------------------------------------------//

void sin(int n, const double* ang, double* sinout);
void cos(int n, const double* ang, double* cosout);
void sincos(int n, const double* ang, double* sinout, double* cosout);
void atan2(int n, const double* y, const double* x, double* ang);
void sqrt(int n, const double* in, double* out);
void rsqrt(int n, const double* in, double* out);
void exp(int n, const double* in, double* out);
void log(int n, const double* in, double* out);

void fast_sin(int n, const double* ang, double* sinout);
void fast_cos(int n, const double* ang, double* cosout);
void fast_sincos(int n, const double* ang, double* sinout, double* cosout);
void fast_atan2(int n, const double* y, const double* x, double* ang);
void fast_sqrt(int n, const double* in, double* out);
void fast_rsqrt(int n, const double* in, double* out);
void fast_exp(int n, const double* in, double* out);
void fast_log(int n, const double* in, double* out);
void fast_erfinv(int n, const double* in, double* out);

//----------------------------------------------------------------------------//
} // namespace func

} // namespace mad

#endif
