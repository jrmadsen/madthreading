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


#include "cov.hh"
#include "memory.hh"
#include "aligned_allocator.hh"
#include "timer.hh"

#include <cstring>
#include <iostream>

#ifdef _OPENMP
#  include <omp.h>
#endif

//============================================================================//

void mad::cov::accumulate_diagonal(int64_t /*nsub*/, int64_t subsize, int64_t nnz,
                                   int64_t nsamp, int64_t const* indx_submap,
                                   int64_t const* indx_pix,
                                   double const* weights,
                                   double scale, double const* signal,
                                   double* zdata, int64_t * hits,
                                   double * invnpp)
{

    #pragma omp parallel default(shared)
    {

        int64_t i, j, k;
        int64_t block = (int64_t)(nnz * (nnz+1) / 2);
        int64_t zpx;
        int64_t hpx;
        int64_t ipx;
        int64_t off;

        int threads = 1;
        int trank = 0;

        #ifdef _OPENMP
        threads = omp_get_num_threads();
        trank = omp_get_thread_num();
        #endif

        int tpix;

        for ( i = 0; i < nsamp; ++i ) {
            if ( ( indx_submap[i] >= 0 ) && ( indx_pix[i] >= 0 ) ) {
                hpx = (indx_submap[i] * subsize) + indx_pix[i];
                tpix = hpx % threads;
                if ( tpix == trank ) {
                    zpx = (indx_submap[i] * subsize * nnz) + (indx_pix[i] * nnz);
                    ipx = (indx_submap[i] * subsize * block) + (indx_pix[i] * block);

                    off = 0;
                    for ( j = 0; j < nnz; ++j ) {
                        zdata[zpx + j] += scale * signal[i] * weights[i * nnz + j];
                        for ( k = j; k < nnz; ++k ) {
                            invnpp[ipx + off] += scale * weights[i * nnz + j] * weights[i * nnz + k];
                            off += 1;
                        }
                    }

                    hits[hpx] += 1;
                }
            }
        }
    }

    return;
}

//============================================================================//

void mad::cov::accumulate_diagonal_hits(int64_t /*nsub*/, int64_t subsize,
                                        int64_t /*nnz*/, int64_t nsamp,
                                        int64_t const* indx_submap,
                                        int64_t const* indx_pix,
                                        int64_t* hits )
{

    #pragma omp parallel default(shared)
    {

        int threads = 1;
        int trank = 0;

        #ifdef _OPENMP
        threads = omp_get_num_threads();
        trank = omp_get_thread_num();
        #endif

        for (int64_t i = 0; i < nsamp; ++i )
        {
            if ( ( indx_submap[i] >= 0 ) && ( indx_pix[i] >= 0 ) )
            {
                int64_t hpx = (indx_submap[i] * subsize) + indx_pix[i];
                int64_t tpix = hpx % threads;
                if ( tpix == trank )
                {
                    hits[hpx] += 1;
                }    
            }
        }

    }

    return;
}

//============================================================================//

void mad::cov::accumulate_diagonal_invnpp ( int64_t /*nsub*/, int64_t subsize,
                                            int64_t nnz, int64_t nsamp,
                                            int64_t const * indx_submap,
                                            int64_t const * indx_pix,
                                            double const * weights,
                                            double scale, int64_t * hits,
                                            double * invnpp ) {

    #pragma omp parallel default(shared)
    {
        int64_t block = (int64_t)(nnz * (nnz+1) / 2);

        int threads = 1;
        int trank = 0;

        #ifdef _OPENMP
        threads = omp_get_num_threads();
        trank = omp_get_thread_num();
        #endif

        for (int64_t i = 0; i < nsamp; ++i )
        {
            if ( ( indx_submap[i] >= 0 ) && ( indx_pix[i] >= 0 ) )
            {
                int64_t hpx = (indx_submap[i] * subsize) + indx_pix[i];
                int64_t tpix = hpx % threads;
                if ( tpix == trank )
                {
                    int64_t ipx = (indx_submap[i] * subsize * block)
                                  + (indx_pix[i] * block);
                    int64_t off = 0;
                    for (int64_t j = 0; j < nnz; ++j )
                    {
                        for (int64_t k = j; k < nnz; ++k )
                        {
                            invnpp[ipx + off] += scale * weights[i * nnz + j]
                                    * weights[i * nnz + k];
                            off += 1;
                        }
                    }
                    hits[hpx] += 1;
                }
            }
        }
    }

    return;
}

//============================================================================//
#define SLOWER
void mad::cov::accumulate_zmap (int64_t /*nsub*/, int64_t subsize, int64_t nnz,
                                int64_t nsamp,
                                int64_t const* indx_submap,
                                int64_t const* indx_pix,
                                double const* weights,
                                double scale,
                                double const* signal,
                                double* zdata )
{
#if defined(SLOWER)
    // > [cxx] ctoast_cov_accumulate_zmap
    // : 118.837 wall,   4.820 user +   4.650 system =   9.470 CPU [seconds] (  8.0%)
    // (total # of laps: 32)
    #pragma omp parallel default(shared)
    {
        int64_t i, j, k;
        int64_t hpx;
        int64_t zpx;

        int threads = 1;
        int trank = 0;

        #ifdef _OPENMP
        threads = omp_get_num_threads();
        trank = omp_get_thread_num();
        #endif

        int tpix;

        for ( i = 0; i < nsamp; ++i ) {
            if ( ( indx_submap[i] >= 0 ) && ( indx_pix[i] >= 0 ) ) {
                hpx = (indx_submap[i] * subsize) + indx_pix[i];
                tpix = hpx % threads;
                if ( tpix == trank ) {
                    zpx = (indx_submap[i] * subsize * nnz) + (indx_pix[i] * nnz);

                    for ( j = 0; j < nnz; ++j ) {
                        zdata[zpx + j] += scale * signal[i] * weights[i * nnz + j];
                    }
                }
            }
        }
    }
#else
    #pragma omp parallel default(shared)
    {
        int threads = 1;
        int trank = 0;

        #ifdef _OPENMP
        threads = omp_get_num_threads();
        trank = omp_get_thread_num();
        #endif

        for (int64_t i = 0; i < nsamp; ++i )
        {
            if ( ( indx_submap[i] >= 0 ) && ( indx_pix[i] >= 0 ) )
            {
                int64_t hpx = (indx_submap[i] * subsize) + indx_pix[i];
                int64_t tpix = hpx % threads;
                if ( tpix == trank )
                {
                    int64_t zpx = (indx_submap[i] * subsize * nnz)
                                  + (indx_pix[i] * nnz);

                    for (int64_t j = 0; j < nnz; ++j )
                    {
                        if(zpx + j > subsize*nnz*nsub)
                        zdata[zpx + j] += scale * signal[i] * weights[i * nnz + j];
                    }
                }
            }
        }
    }
#endif
    return;
}

//============================================================================//

void mad::cov::accumulate_zmap_input(int rank, int fc)
{
    int64_t nsub;
    int64_t subsize;
    int64_t nnz;
    int64_t nsamp;
    double scale;

    std::stringstream suffix; suffix << "_" << rank << "_" << fc;
    {
        std::string fmeta = "data/metadata";
        fmeta += suffix.str();
        fmeta += ".out";
        std::ifstream fis(fmeta.c_str());
        fis >> nsub >> subsize >> nnz >> nsamp >> scale;
        fis.close();
    }

    int64_t* indx_submap = input<int64_t>("data/indx_submap" + suffix.str());
    int64_t* indx_pix = input<int64_t>("data/indx_pix" + suffix.str());
    double* weights = input<double>("data/weights" + suffix.str());
    double* signal = input<double>("data/signal" + suffix.str());
    double* zdata = input<double>("data/zdata" + suffix.str());

    accumulate_zmap_output(nsub, subsize, nnz, nsamp, indx_submap, indx_pix,
                           weights, scale, signal, zdata, rank, fc);

    std::stringstream prefix;
    prefix << "[cxx] " << __FUNCTION__  << "("
           << std::setw(2) << rank << ", "
           << std::setw(2) << fc << ") [ ";

    mad::util::timer t(3, prefix.str());
    t.start();

    accumulate_zmap(nsub, subsize, nnz, nsamp, indx_submap, indx_pix,
                    weights, scale, signal, zdata);

    t.stop();
    t.report();
    std::cout << std::endl;
}

//============================================================================//

void mad::cov::accumulate_zmap_output(int64_t nsub, int64_t subsize, int64_t nnz,
                                      int64_t nsamp,
                                      int64_t const* indx_submap,
                                      int64_t const* indx_pix,
                                      double const* weights,
                                      double scale,
                                      double const* signal,
                                      double* zdata,
                                      int rank, int fc)
{
    std::stringstream suffix;
    suffix << "_" << rank << "_" << fc;
    std::string fmeta = "metadata";
    fmeta += suffix.str();
    fmeta += ".out";
    std::ofstream fos(fmeta.c_str());
    fos.precision(15);
    fos << std::scientific
        << std::setw(40) << nsub << " \n"
        << std::setw(40) << subsize << " \n"
        << std::setw(40) << nnz << " \n"
        << std::setw(40) << nsamp << " \n"
        << std::setw(40) << scale << " \n"
        << std::setw(40) << "indx_submap" << " \n"
        << std::setw(40) << "indx_pix" << " \n"
        << std::setw(40) << "weights" << " \n"
        << std::setw(40) << "signal" << " \n"
        << std::setw(40) << "zdata" << " \n"
        << std::endl;
    fos.close();

    output<int64_t>("indx_submap" + suffix.str(), indx_submap, nsamp);
    output<int64_t>("indx_pix" + suffix.str(), indx_pix, nsamp);
    output<double>("weights" + suffix.str(), weights, nsamp*nnz);
    output<double>("signal" + suffix.str(), signal, nsamp);
    output<double>("zdata" + suffix.str(), zdata, subsize*nnz*nsub);
}

//============================================================================//

void mad::cov::apply_diagonal(int64_t nsub, int64_t subsize, int64_t nnz,
                              double const* mat, double* vec )
{

    int64_t block = (int64_t)(nnz * (nnz+1) / 2);

    if ( nnz == 1 )
    {
        // shortcut for NNZ == 1

        for (int64_t i = 0; i < nsub; ++i )
        {
            for (int64_t j = 0; j < subsize; ++j )
            {
                int64_t mpx = (i * subsize * block) + (j * block);
                int64_t vpx = (i * subsize * nnz) + (j * nnz);
                vec[vpx] *= mat[mpx];
            }
        }

    }
    else
    {
        // We do this manually now, but could use dsymv if needed...
        // Since this is just multiply / add operations, the overhead of threading
        // is likely more than the savings.

        int64_t m;
        int64_t off;

        mad::simd_array<double> temp(nnz);

        for (int64_t  i = 0; i < nsub; ++i )
        {
            for (int64_t j = 0; j < subsize; ++j )
            {
                int64_t mpx = (i * subsize * block) + (j * block);
                int64_t vpx = (i * subsize * nnz) + (j * nnz);

                ::memset(temp, 0, nnz * sizeof(double));

                off = 0;
                for (int64_t k = 0; k < nnz; ++k )
                {
                    for ( m = k; m < nnz; ++m )
                    {
                        temp[k] += mat[mpx + off] * vec[vpx + m];
                        if ( m != k )
                        {
                            temp[m] += mat[mpx + off] * vec[vpx + k];
                        }
                        off++;
                    }
                }

                for (int64_t k = 0; k < nnz; ++k ) {
                    vec[vpx + k] = temp[k];
                }
            }
        }
    }

    return;
}

//============================================================================//

