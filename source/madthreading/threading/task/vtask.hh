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

#ifndef vtask_hh_
#define vtask_hh_

#include "madthreading/macros.hh"
#include "madthreading/threading/threading.hh"
#include "madthreading/threading/auto_lock.hh"
#include "madthreading/allocator/allocator.hh"
#include "madthreading/threading/condition.hh"
#include "madthreading/threading/auto_lock.hh"
#include "madthreading/threading/mutex.hh"
#include "madthreading/types.hh"

#include <functional>
#include <utility>
#include <tuple>
#include <cstddef>
#include <string>
#include <array>
#include <future>
#include <thread>

namespace mad
{

namespace details
{

class vtask_group;

//============================================================================//

/// \brief vtask is the abstract class stored in thread_pool
class vtask : public mad::Allocator
{
public:
    typedef vtask                   this_type;
    typedef vtask*                  iterator;
    typedef const vtask*            const_iterator;
    typedef size_t                  size_type;
    typedef std::atomic_uint64_t    count_t;
    //typedef Allocator<this_type>    allocator_type;

public:
    vtask(vtask_group* _group = nullptr) : m_vgroup(_group) { }
    virtual ~vtask() { }

public:
    // execution operator
    virtual void operator()() = 0;

public:
    // used by thread_pool
    void operator++();
    void operator--();

public:
    // used by task tree
    iterator begin() { return this; }
    iterator end() { return this+1; }

    const_iterator begin() const { return this; }
    const_iterator end() const { return this+1; }

    const_iterator cbegin() const { return this; }
    const_iterator cend() const { return this+1; }

protected:
    vtask_group* m_vgroup;

};

//============================================================================//

} // details

} // mad

#endif

