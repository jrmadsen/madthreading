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

#ifndef cov_hh_
#define cov_hh_

#include <cstdint>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <atomic>
#include <iostream>

namespace mad 
{ 

namespace cov 
{

//----------------------------------------------------------------------------//

void accumulate_diagonal(int64_t nsub, int64_t subsize, int64_t nnz,
                         int64_t nsamp,
                         int64_t const* indx_submap,
                         int64_t const* indx_pix,
                         double const* weights,
                         double scale,
                         double const* signal,
                         double* zdata, int64_t* hits,
                         double* invnpp );

//----------------------------------------------------------------------------//

void accumulate_diagonal_hits(int64_t nsub, int64_t subsize, int64_t nnz,
                              int64_t nsamp,
                              int64_t const* indx_submap,
                              int64_t const* indx_pix,
                              int64_t * hits );

//----------------------------------------------------------------------------//

void accumulate_diagonal_invnpp(int64_t nsub, int64_t subsize, int64_t nnz,
                                int64_t nsamp,
                                int64_t const* indx_submap,
                                int64_t const* indx_pix,
                                double const* weights,
                                double scale, int64_t* hits,
                                double* invnpp );

//----------------------------------------------------------------------------//

void accumulate_zmap(int64_t nsub, int64_t subsize, int64_t nnz,
                     int64_t nsamp,
                     int64_t const* indx_submap,
                     int64_t const* indx_pix,
                     double const* weights,
                     double scale,
                     double const* signal,
                     double* zdata );

//----------------------------------------------------------------------------//

void accumulate_zmap_input(int nr, int nc);

//----------------------------------------------------------------------------//

void accumulate_zmap_output(int64_t nsub, int64_t subsize, int64_t nnz,
                            int64_t nsamp,
                            int64_t const* indx_submap,
                            int64_t const* indx_pix,
                            double const* weights,
                            double scale,
                            double const* signal,
                            double* zdata,
                            int rank, int fc);

//----------------------------------------------------------------------------//

void apply_diagonal(int64_t nsub, int64_t subsize, int64_t nnz,
                    double const* mat, double* vec );

//----------------------------------------------------------------------------//

template <typename _Tp>
void output(std::string fname, const _Tp* data, size_t n)
{
    fname += ".out";
    std::ofstream fos(fname.c_str(), std::ofstream::binary);
    fos.write((char*) data, n * sizeof(_Tp));
    fos.close();
}

//----------------------------------------------------------------------------//

template <typename _Tp>
_Tp* input(std::string fname)
{
    fname += ".out";
#ifdef DEBUG
    std::cout << "Opening " << fname << "..." << std::endl;
#endif
    std::ifstream fis(fname.c_str(), std::ifstream::binary);

    if(!fis)
    {
        std::stringstream ss;
        ss << "Error! Unable to open input file : " << fname;
        throw std::runtime_error(ss.str());
    }

    // get size of file
    fis.seekg (0, fis.end);
    int64_t size = fis.tellg() / sizeof(_Tp);
    fis.seekg (0);

    // allocate memory for file content
    _Tp* data = new _Tp[size];

    // read content of infile
    fis.read((char*) data, size * sizeof(_Tp));

    return data;
}

//----------------------------------------------------------------------------//

} // namespace cov

} // namespace mad

#endif

