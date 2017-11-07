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
// created by jmadsen on Thu Apr 21 15:58:28 2016
//
//
//
//

#ifndef unittest_hh_
#define unittest_hh_

#include <UnitTest++/UnitTest++.h>
#include <UnitTest++/TestReporterStdout.h>

//----------------------------------------------------------------------------//

int RunAllTests(UnitTest::TestReporter& reporter,
                char const* suiteName = NULL,
                int const maxTestTimeInMs = 0 )
{
    UnitTest::TestRunner runner( reporter );
    return runner.RunTestsIf( UnitTest::Test::GetTestList(), suiteName,
                              UnitTest::True(), maxTestTimeInMs);
}

//----------------------------------------------------------------------------//

int RunBySuites(int argc, char** argv, UnitTest::TestReporter& reporter)
{
    int error = 0;
    for(int i = 1; i < argc; ++i)
        error += RunAllTests(reporter, argv[i]);

    return error;
}

//----------------------------------------------------------------------------//

int RunTests(int argc, char** argv, UnitTest::TestReporter& reporter)
{
    bool UseSuites = (argc > 1);

    if(UseSuites)
      return RunBySuites( argc, argv, reporter );

    return RunAllTests(reporter);
}

//----------------------------------------------------------------------------//

#endif /* UNITTEST_HH_*/
