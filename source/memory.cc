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
// created by jmadsen on Sat Jul 15 16:17:20 2017
//
//
//
//


#include "memory.hh"
#include "exception.hh"

//============================================================================//

void* mad::memory::aligned_alloc(size_t size, size_t align)
{
    void* mem = NULL;
    int ret = posix_memalign(&mem, align, size);
    if (ret != 0)
    {
        std::ostringstream o;
        o << "cannot allocate " << size
          << " bytes of memory with alignment " << align;
        MAD_THROW(o.str().c_str());
    }
    memset(mem, 0, size);
    return mem;
}

//============================================================================//

void mad::memory::aligned_free(void* ptr)
{
    free (ptr);
    return;
}

//============================================================================//


