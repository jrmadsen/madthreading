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
// created by jmadsen on Sat Jul 15 17:03:25 2017
//
//
//
//

#ifndef array_hh_
#define array_hh_

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwchar>

//============================================================================//

namespace mad
{

namespace array
{


//----------------------------------------------------------------------------//

using std::size_t;

void list_dot(size_t n, size_t m, size_t d, const double* a,
              const double* b, double* dotprod);

void inv(size_t n, double* q);

void amplitude(size_t n, size_t m, size_t d, const double* v,
               double* norm);

void normalize(size_t n, size_t m, size_t d, const double* q_in,
               double* q_out);

void normalize_inplace(size_t n, size_t m, size_t d, double* q);

void rotate(size_t nq, const double* q, size_t nv, const double* v_in,
            double* v_out);

void mult(size_t np, const double* p, size_t nq, const double* q, double* r);

void slerp(size_t n_time, size_t n_targettime, const double* time,
           const double* targettime, const double* q_in, double* q_interp);

void exp(size_t n, const double* q_in, double* q_out);

void ln(size_t n, const double* q_in, double* q_out);

void pow(size_t n, const double* p, const double* q_in,
         double* q_out);

void from_axisangle(size_t n, const double* axis, const double* angle,
                    double* q_out);

void to_axisangle(size_t n, const double* q, double* axis,
                  double* angle);

void to_rotmat(const double* q, double* rotmat);

void from_rotmat(const double* rotmat, double* q);

void from_vectors(const double* vec1, const double* vec2, double* q);

void from_angles(size_t n, const double* theta, const double* phi,
                 const double* pa, double* quat, bool IAU = false);

void to_angles(size_t n, const double* quat, double* theta,
               double* phi, double* pa, bool IAU = false);

} // namespace array

} // namespace mad

//----------------------------------------------------------------------------//

#endif
