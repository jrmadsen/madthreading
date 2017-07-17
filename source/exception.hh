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
// created by jmadsen on Sat Jul 15 16:26:30 2017
//
//
//
//

#ifndef exception_hh_
#define exception_hh_

#include <new>
#include <cstring>
#include <sstream>

//----------------------------------------------------------------------------//

namespace mad
{

// Exception handling
class exception : public std::exception
{
public:
    exception(const char* msg, const char* file, int line);
    ~exception() throw ();
    const char* what() const throw();

private:
    // use C strings here for passing to what()
    static size_t const msg_len_ = 1024;
    char msg_[ msg_len_ ];
};

typedef void (*MAD_EXCEPTION_HANDLER) (mad::exception& e);

#define MAD_THROW(msg) \
throw mad::exception (msg, __FILE__, __LINE__ )

#define MAD_TRY \
try {

#define MAD_CATCH \
} catch (mad::exception& e) { \
    std::cerr << e.what() << std::endl; \
    mad::cleanup(); \
    throw; \
}

#define mad_CATCH_CUSTOM(handler) \
} catch (mad::exception& e) { \
    (*handler)(e); \
}

}

//----------------------------------------------------------------------------//

#endif
