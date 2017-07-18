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
// created by jmadsen on Tue Jul 18 01:12:41 2017
//
//
//
//

#ifndef tools_hh_
#define tools_hh_

#include "macros.hh"
#include "exception.hh"

//============================================================================//

namespace mad
{
namespace tools
{
//----------------------------------------------------------------------------//

inline bool mkdir(const std::string& str)
{
    int r = 0;
    std::string cmd = "if [ ! -d " + str + " ]; then mkdir -p " + str + "; fi";
    if(system(NULL))
    {
        r = system(cmd.c_str());
        if(r == 0)
            return true;
        else
            MAD_THROW("Unable to create directory");
    }
    else
    {
        MAD_THROW("Unable to create directory - system is not available");
    }
    return false;
}

//----------------------------------------------------------------------------//
} // namespace tools

} // namespace mad



#endif
