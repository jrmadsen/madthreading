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
// created by jmadsen on Fri Aug  7 09:44:32 2015
//
//
//
//


#ifndef allocator_pool_hh_
#define allocator_pool_hh_

//----------------------------------------------------------------------------//
#ifdef SWIG
%module allocator_pool
%{
    #include "madthreading/allocator/allocator_pool.hh"
%}
#endif
//----------------------------------------------------------------------------//

#include <cstddef>

namespace mad
{
namespace details
{

class allocator_pool
{
public:
    typedef std::size_t size_type;

public:
    // Constructor and Destructors
    explicit allocator_pool(size_type n = 0);
    // Virtual destructors are required by abstract classes
    // so add it by default, just in case
    ~allocator_pool();

public:
    // Public functions
    // allocate one element
    inline void* alloc();
    // free
    inline void free(void*);

public:
    inline size_type size() const;
    void reset();
    inline long get_num_pages() const;
    inline size_type get_page_size() const;
    inline void grow_page_size(size_type factor);

private:
    // Private functions
    allocator_pool(const allocator_pool&);
    allocator_pool& operator=(const allocator_pool&);

private:
    struct pool_link
    {
        pool_link* next;
    };

private:
    class pool_chunk
    {
    public:
        typedef std::size_t size_type;

    public:
        explicit pool_chunk(size_type sz)
        : size(sz), mem(new char[size]), next(0)
        { }
        ~pool_chunk() { delete [] mem; }

        const size_type size;
        char* mem;
        pool_chunk* next;
    };

private:
    // make pool larger
    void grow();

private:
    // Private variables
    const size_type m_esize;
    size_type m_csize;
    pool_chunk* m_chunks;
    pool_link*  m_head;
    long m_nchunks;

};

// ------------------------------------------------------------
// Inline implementation
// ------------------------------------------------------------

// ************************************************************
// Alloc
// ************************************************************
//
inline void*
allocator_pool::alloc()
{
    if (m_head == 0) { grow(); }
    pool_link* p = m_head;  // return first element
    m_head = p->next;
    return p;
}

// ************************************************************
// Free
// ************************************************************
//
inline void
allocator_pool::free(void* b)
{
    pool_link* p = static_cast<pool_link*>(b);
    p->next = m_head;        // put b back as first element
    m_head = p;
}

//----------------------------------------------------------------------------//
// Size
//----------------------------------------------------------------------------//
//
inline allocator_pool::size_type
allocator_pool::size() const
{
    return m_nchunks*m_csize;
}

//----------------------------------------------------------------------------//
// GetNoPages
//----------------------------------------------------------------------------//
//
inline long
allocator_pool::get_num_pages() const
{
    return m_nchunks;
}

//----------------------------------------------------------------------------//
// GetPageSize
//----------------------------------------------------------------------------//
//
inline allocator_pool::size_type
allocator_pool::get_page_size() const
{
    return m_csize;
}

//----------------------------------------------------------------------------//
// GrowPageSize
//----------------------------------------------------------------------------//
//
inline void
allocator_pool::grow_page_size(size_type factor)
{
    m_csize = (factor) ? factor*m_csize : m_csize;
}

//----------------------------------------------------------------------------//

} // namespace details
} // namespace mad

#endif
