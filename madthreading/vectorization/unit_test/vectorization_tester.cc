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


#include <UnitTest++.h>

#include <iomanip>
#include <iostream>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwchar>
#include <cmath>

#include "madthreading/vectorization/array.hh"
#include "madthreading/allocator/aligned_allocator.hh"
#include "madthreading/utility/memory.hh"
#include "madthreading/utility/constants.hh"
#include "madthreading/vectorization/func.hh"

#include <vector>

using namespace mad;
using namespace mad::dat;

#define EXPECT_DOUBLE_EQ( a, b ) CHECK_CLOSE( a, b, std::numeric_limits<double>::epsilon())
#define EXPECT_FLOAT_EQ( a, b ) CHECK_CLOSE( a, b, std::numeric_limits<float>::epsilon())

SUITE( Vectorization_Tests )
{
    const double q1[] = { 0.50487417,  0.61426059,  0.60118994,  0.07972857 };
    const double q1inv[] = { -0.50487417,  -0.61426059,  -0.60118994,  0.07972857 };
    const double q2[] = { 0.43561544,  0.33647027,  0.40417115,  0.73052901 };
    const double qtonormalize[] = { 1.0, 2.0, 3.0, 4.0, 2.0, 3.0, 4.0, 5.0 };
    const double qnormalized[] = { 0.18257419, 0.36514837, 0.54772256, 0.73029674, 0.27216553, 0.40824829, 0.54433105, 0.68041382 };
    const double vec[] = { 0.57734543, 0.30271255, 0.75831218 };
    const double vec2[] = { 0.57734543, 8.30271255, 5.75831218, 1.57734543, 3.30271255, 0.75831218 };
    const double qeasy[] = { 0.3, 0.3, 0.1, 0.9, 0.3, 0.3, 0.1, 0.9 };
    const double mult_result[] = { 0.44954009, 0.53339352, 0.37370443, -0.61135101 };
    const double rot_by_q1[] = { 0.4176698, 0.84203849, 0.34135482 };
    const double rot_by_q2[] = { 0.8077876, 0.3227185, 0.49328689 };

    TEST( arraylist_dot1 )
    {
        double check;
        double result;
        double pone[3];

        check = 0.0;
        for ( size_t i = 0; i < 3; ++i ) {
            pone[i] = vec[i] + 1.0;
            check += vec[i] * pone[i];
        }

        array::list_dot ( 1, 3, 3, vec, pone, &result );

        EXPECT_DOUBLE_EQ( check, result );
    }


    TEST( arraylist_dot2 )
    {
        double check[2];
        double result[2];
        double pone[6];

        for ( size_t i = 0; i < 2; ++i ) {
            check[i] = 0.0;
            for ( size_t j = 0; j < 3; ++j ) {
                pone[3*i+j] = vec2[3*i+j] + 1.0;
                check[i] += vec2[3*i+j] * pone[3*i+j];
            }
        }

        array::list_dot ( 2, 3, 3, vec2, pone, result );

        for ( size_t i = 0; i < 2; ++i ) {
            EXPECT_DOUBLE_EQ( check[i], result[i] );
        }
    }


    TEST( inv )
    {
        double result[4];

        for ( size_t i = 0; i < 4; ++i ) {
            result[i] = q1[i];
        }

        array::inv ( 1, result );

        for ( size_t i = 0; i < 4; ++i ) {
            EXPECT_FLOAT_EQ( q1inv[i], result[i] );
        }
    }


    TEST( norm )
    {
        double result[4];

        array::normalize ( 1, 4, 4, qtonormalize, result );

        for ( size_t i = 0; i < 4; ++i ) {
            EXPECT_FLOAT_EQ( qnormalized[i], result[i] );
        }
    }


    TEST( mult )
    {
        double result[4];

        array::mult ( 1, q1, 1, q2, result );

        for ( size_t i = 0; i < 4; ++i ) {
            EXPECT_FLOAT_EQ( mult_result[i], result[i] );
        }
    }


    TEST( multarray )
    {
        size_t n = 3;
        mad::simd_array<double> in1(4*n);
        mad::simd_array<double> in2(4*n);
        mad::simd_array<double> result(4*n);
        double null[4];

        null[0] = 0.0;
        null[1] = 0.0;
        null[2] = 0.0;
        null[3] = 1.0;

        for ( size_t i = 0; i < n; ++i )
        {
            for ( size_t j = 0; j < 4; ++j )
            {
                in1[4*i+j] = q1[j];
                in2[4*i+j] = q2[j];
            }
        }

        array::mult ( n, in1, n, in2, result );

        for ( size_t i = 0; i < n; ++i )
        {
            for ( size_t j = 0; j < 4; ++j )
            {
                EXPECT_FLOAT_EQ( mult_result[j], result[4*i+j] );
            }
        }

        array::mult ( n, in1, 1, null, result );

        for ( size_t i = 0; i < n; ++i ) {
            for ( size_t j = 0; j < 4; ++j ) {
                EXPECT_FLOAT_EQ( in1[j], result[4*i+j] );
            }
        }
    }


    TEST( rot1 )
    {
        double result[3];

        array::rotate ( 1, q1, 1, vec, result );

        for ( size_t i = 0; i < 3; ++i ) {
            EXPECT_FLOAT_EQ( rot_by_q1[i], result[i] );
        }
    }


    TEST( rotarray )
    {
        size_t n = 2;
        mad::simd_array<double> qin(4*n);
        mad::simd_array<double> vin(3*n);
        mad::simd_array<double> result(3*n);

        for ( size_t i = 0; i < 4; ++i ) {
            qin[i] = q1[i];
            qin[4+i] = q2[i];
        }

        for ( size_t i = 0; i < n; ++i ) {
            for ( size_t j = 0; j < 3; ++j ) {
                vin[3*i+j] = vec[j];
            }
        }

        array::rotate ( n, qin, n, vin, result );

        for ( size_t i = 0; i < 3; ++i ) {
            EXPECT_FLOAT_EQ( rot_by_q1[i], result[i] );
            EXPECT_FLOAT_EQ( rot_by_q2[i], result[3+i] );
        }
    }


    TEST( slerp )
    {
        size_t n = 2;
        size_t ninterp = 4;

        double q[8] = { 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0 };
        double qinterp[16];
        double time[2] = { 0.0, 9.0 };
        double targettime[4] = { 0.0, 3.0, 4.5, 9.0 };
        double qcheck1[4];
        double qcheck2[4];

        array::normalize_inplace ( n, 4, 4, q );

        array::slerp ( n, ninterp, time, targettime, q, qinterp );

        for ( size_t i = 0; i < 4; ++i ) {
            qcheck1[i] = (2.0 / 3.0) * q[i] + (1.0 / 3.0) * q[4+i];
            qcheck2[i] = 0.5 * (q[i] + q[4+i]);
        }

        array::normalize_inplace( 1, 4, 4, qcheck1 );
        array::normalize_inplace( 1, 4, 4, qcheck2 );

        for ( size_t i = 0; i < 4; ++i ) {
            EXPECT_FLOAT_EQ( q[i], qinterp[i] );
            EXPECT_FLOAT_EQ( q[4+i], qinterp[12+i] );
            CHECK_CLOSE( qcheck1[i], qinterp[4+i], 1.0e-4 );
            CHECK_CLOSE( qcheck2[i], qinterp[8+i], 1.0e-4 );
        }
    }


    TEST( rotation )
    {
        double result[4];
        double axis[3] = { 0.0, 0.0, 1.0 };
        double ang = PI * 30.0 / 180.0;

        array::from_axisangle ( 1, axis, &ang, result );

        EXPECT_FLOAT_EQ( 0.0, result[0] );
        EXPECT_FLOAT_EQ( 0.0, result[1] );
        EXPECT_FLOAT_EQ( ::sin(15.0 * PI / 180.0), result[2] );
        EXPECT_FLOAT_EQ( ::cos(15.0 * PI / 180.0), result[3] );
    }


    TEST( toaxisangle )
    {
        double in[4] = { 0.0, 0.0, ::sin(15.0 * PI / 180.0), ::cos(15.0 * PI / 180.0) };
        double axis[3];
        double ang;
        double checkaxis[3] = { 0.0, 0.0, 1.0 };
        double checkang = 30.0 * PI / 180.0;

        array::to_axisangle ( 1, in, axis, &ang );

        EXPECT_FLOAT_EQ( checkang, ang );
        for ( size_t i = 0; i < 3; ++i ) {
            EXPECT_FLOAT_EQ( checkaxis[i], axis[i] );
        }
    }


    TEST( exp )
    {
        double result[8];
        double check[8] = { 0.71473568, 0.71473568, 0.23824523, 2.22961712, 0.71473568, 0.71473568, 0.23824523, 2.22961712 };

        array::exp ( 2, qeasy, result );

        for ( size_t i = 0; i < 8; ++i ) {
            EXPECT_FLOAT_EQ( check[i], result[i] );
        }
    }


    TEST( ln )
    {
        double result[8];
        double check[8] = { 0.31041794, 0.31041794, 0.10347265, 0.0, 0.31041794, 0.31041794, 0.10347265, 0.0 };

        array::ln ( 2, qeasy, result );

        for ( size_t i = 0; i < 8; ++i ) {
            EXPECT_FLOAT_EQ( check[i], result[i] );
        }
    }


    TEST( pow )
    {
        double p[2];
        double result[8];
        double check1[8] = { 0.672, 0.672, 0.224, 0.216, 0.672, 0.672, 0.224, 0.216 };
        double check2[8] = { 0.03103127, 0.03103127, 0.01034376, 0.99898305, 0.03103127, 0.03103127, 0.01034376, 0.99898305 };

        p[0] = 3.0;
        p[1] = 3.0;
        array::pow ( 2, p, qeasy, result );

        for ( size_t i = 0; i < 8; ++i ) {
            EXPECT_FLOAT_EQ( check1[i], result[i] );
        }

        p[0] = 0.1;
        p[1] = 0.1;
        array::pow ( 2, p, qeasy, result );

        for ( size_t i = 0; i < 8; ++i ) {
            EXPECT_FLOAT_EQ( check2[i], result[i] );
        }
    }


    TEST( torotmat )
    {
        double result[9];
        double check[9] = { 8.00000000e-01, -2.77555756e-17, 6.00000000e-01, 3.60000000e-01, 8.00000000e-01, -4.80000000e-01, -4.80000000e-01, 6.00000000e-01, 6.40000000e-01 };

        array::to_rotmat ( qeasy, result );

        for ( size_t i = 0; i < 9; ++i ) {
            if ( ::fabs ( check[i] ) > 1.0e-12 ) {
                EXPECT_FLOAT_EQ( check[i], result[i] );
            }
        }
    }


    TEST( fromrotmat )
    {
        double result[9];
        double qresult[4];

        array::to_rotmat ( qeasy, result );
        array::from_rotmat ( result, qresult );

        for ( size_t i = 0; i < 4; ++i ) {
            EXPECT_FLOAT_EQ( qeasy[i], qresult[i] );
        }
    }


    TEST( fromvectors )
    {
        double result[4];
        double check[4] = { 0.0, 0.0, ::sin(15.0 * PI / 180.0), ::cos(15.0 * PI / 180.0) };
        double ang = 30.0 * PI / 180.0;
        double v1[3] = { 1.0, 0.0, 0.0 };
        double v2[3] = { ::cos(ang), ::sin(ang), 0.0 };

        array::from_vectors ( v1, v2, result );

        for ( size_t i = 0; i < 4; ++i ) {
            EXPECT_FLOAT_EQ( check[i], result[i] );
        }
    }


    TEST( thetaphipa )
    {
        size_t n_theta = 5;
        size_t n_phi = 5;
        size_t n = n_theta * n_phi;

        double xaxis[3] = { 1.0, 0.0, 0.0 };
        double zaxis[3] = { 0.0, 0.0, 1.0 };

        mad::simd_array<double> theta(n);
        mad::simd_array<double> phi(n);
        mad::simd_array<double> pa(n);

        mad::simd_array<double> check_theta(n);
        mad::simd_array<double> check_phi(n);
        mad::simd_array<double> check_pa(n);

        mad::simd_array<double> quat(4*n);

        // First run tests in Healpix convention...

        for ( size_t i = 0; i < n_theta; ++i ) {
            for ( size_t j = 0; j < n_phi; ++j ) {
                theta[i*n_phi + j] = (0.5 + (double)i) * PI / (double)n_theta;
                phi[i*n_phi + j] = (double)j * TWOPI / (double)n_phi;
                pa[i*n_phi + j] = (double)j * TWOPI / (double)n_phi - PI;
            }
        }

        // convert to quaternions

        array::from_angles ( n, theta, phi, pa, quat, false );

        // check that the resulting quaternions rotate the Z and X
        // axes to the correct place.

        double dir[3];
        double orient[3];
        double check;

        for ( size_t i = 0; i < n; ++i ) {
            array::rotate ( 1, &(quat[4*i]), 1, zaxis, dir );
            array::rotate ( 1, &(quat[4*i]), 1, xaxis, orient );

            CHECK_CLOSE( PI_2 - ::asin(dir[2]), theta[i], 1.0e-6 );

            check = ::atan2 ( dir[1], dir[0] );

            if ( check < 0.0 ) {
                check += TWOPI;
            }
            if ( check >= TWOPI ) {
                check -= TWOPI;
            }
            if ( ::fabs( check ) < 2.0 * std::numeric_limits<float>::epsilon() ) {
                check = 0.0;
            }
            if ( ::fabs( check - TWOPI ) < 2.0 * std::numeric_limits<float>::epsilon() ) {
                check = 0.0;
            }

            CHECK_CLOSE( check, phi[i], 1.0e-6 );

            check = ::atan2 ( orient[0] * dir[1] - orient[1] * dir[0],
                    - ( orient[0] * dir[2] * dir[0] )
                    - ( orient[1] * dir[2] * dir[1] )
                    + ( orient[2] * ( dir[0] * dir[0] + dir[1] * dir[1] ) ) );

            CHECK_CLOSE( check, pa[i], 1.0e-6 );
        }

        array::to_angles ( n, quat, check_theta, check_phi, check_pa, false );

        for ( size_t i = 0; i < n; ++i ) {
            CHECK_CLOSE( theta[i], check_theta[i], 1.0e-6 );

            check = check_phi[i];
            if ( check < 0.0 ) {
                check += TWOPI;
            }
            if ( check >= TWOPI ) {
                check -= TWOPI;
            }
            if ( ::fabs( check ) < 2.0 * std::numeric_limits<float>::epsilon() ) {
                check = 0.0;
            }
            if ( ::fabs( check - TWOPI ) < 2.0 * std::numeric_limits<float>::epsilon() ) {
                check = 0.0;
            }

            CHECK_CLOSE( phi[i], check, 1.0e-6 );

            CHECK_CLOSE( pa[i], check_pa[i], 1.0e-6 );
        }

        // Now run tests in IAU convention...

        for ( size_t i = 0; i < n_theta; ++i ) {
            for ( size_t j = 0; j < n_phi; ++j ) {
                theta[i*n_phi + j] = (0.5 + (double)i) * PI / (double)n_theta;
                phi[i*n_phi + j] = (double)j * TWOPI / (double)n_phi;
                pa[i*n_phi + j] = - (double)j * TWOPI / (double)n_phi + PI;
            }
        }

        // convert to quaternions

        array::from_angles ( n, theta, phi, pa, quat, true );

        // check that the resulting quaternions rotate the Z and X
        // axes to the correct place.

        for ( size_t i = 0; i < n; ++i ) {
            array::rotate ( 1, &(quat[4*i]), 1, zaxis, dir );
            array::rotate ( 1, &(quat[4*i]), 1, xaxis, orient );

            CHECK_CLOSE( PI_2 - ::asin (dir[2]), theta[i], 1.0e-6 );

            check = ::atan2 ( dir[1], dir[0] );

            if ( check < 0.0 ) {
                check += TWOPI;
            }
            if ( check >= TWOPI ) {
                check -= TWOPI;
            }
            if ( ::fabs( check ) < 2.0 * std::numeric_limits<float>::epsilon() ) {
                check = 0.0;
            }
            if ( ::fabs( check - TWOPI ) < 2.0 * std::numeric_limits<float>::epsilon() ) {
                check = 0.0;
            }

            CHECK_CLOSE( check, phi[i], 1.0e-6 );

            check = - ::atan2 ( orient[0] * dir[1] - orient[1] * dir[0],
                    - ( orient[0] * dir[2] * dir[0] )
                    - ( orient[1] * dir[2] * dir[1] )
                    + ( orient[2] * ( dir[0] * dir[0] + dir[1] * dir[1] ) ) );

            if ( ::fabs ( ::fabs ( check - pa[i] ) - TWOPI ) < std::numeric_limits<float>::epsilon() ) {
                // we are at the same angle, just with 2PI rotation.
            } else if ( ::fabs ( ::fabs ( pa[i] - check ) - TWOPI ) < std::numeric_limits<float>::epsilon() ) {
                // we are at the same angle, just with 2PI rotation.
            } else {
                CHECK_CLOSE( check, pa[i], 1.0e-6 );
            }
        }

        array::to_angles ( n, quat, check_theta, check_phi, check_pa, true );

        for ( size_t i = 0; i < n; ++i ) {
            CHECK_CLOSE( theta[i], check_theta[i], 1.0e-6 );

            check = check_phi[i];
            if ( check < 0.0 ) {
                check += TWOPI;
            }
            if ( check >= TWOPI ) {
                check -= TWOPI;
            }
            if ( ::fabs( check ) < 2.0 * std::numeric_limits<float>::epsilon() ) {
                check = 0.0;
            }
            if ( ::fabs( check - TWOPI ) < 2.0 * std::numeric_limits<float>::epsilon() ) {
                check = 0.0;
            }

            CHECK_CLOSE( phi[i], check, 1.0e-6 );

            CHECK_CLOSE( pa[i], check_pa[i], 1.0e-6 );
        }
    }

}
