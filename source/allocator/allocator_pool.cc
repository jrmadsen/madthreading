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
// created by jmadsen on Fri Aug  7 09:44:32 2015
//
//
//
//


#include "allocator_pool.hh"

namespace mad
{
namespace details
{

//============================================================================//

allocator_pool::allocator_pool(size_type sz)
: m_esize(sz < sizeof(pool_link) ? sizeof(pool_link) : sz),
  m_csize(sz<1024/2-16 ? 1024-16 : sz*10-16),
  m_chunks(0), m_head(0), m_nchunks(0)
{

}

//============================================================================//

allocator_pool::~allocator_pool()
{
    reset();
}

//============================================================================//

allocator_pool::allocator_pool(const allocator_pool& rhs)
: m_esize(rhs.m_esize), m_csize(rhs.m_csize),
  m_chunks(rhs.m_chunks), m_head(rhs.m_head), m_nchunks(rhs.m_nchunks)
{

}

//============================================================================//

allocator_pool& allocator_pool::operator=(const allocator_pool& rhs)
{
    if(&rhs != this)
    {
        m_chunks  = rhs.m_chunks;
        m_head    = rhs.m_head;
        m_nchunks = rhs.m_nchunks;
    }
    return *this;
}

//============================================================================//

void allocator_pool::reset()
{
    // Free all chunks
    //
    pool_chunk* n = m_chunks;
    pool_chunk* p = 0;
    while (n)
    {
        p = n;
        n = n->next;
        delete p;
    }
    m_head = 0;
    m_chunks = 0;
    m_nchunks = 0;
}

//============================================================================//

void allocator_pool::grow()
{
    // Allocate new chunk, organize it as a linked list of
    // elements of size 'esize'
    //
    pool_chunk* n = new pool_chunk(m_csize);
    n->next = m_chunks;
    m_chunks = n;
    m_nchunks++;

    const int nelem = m_csize/m_esize;
    char* start = n->mem;
    char* last = &start[(nelem-1)*m_esize];
    for (char* p = start; p < last; p += m_esize)
    {
        reinterpret_cast<pool_link*>(p)->next
                = reinterpret_cast<pool_link*>(p+m_esize);
    }
    reinterpret_cast<pool_link*>(last)->next = 0;
    m_head = reinterpret_cast<pool_link*>(start);
}

//============================================================================//

} // namespace details
} // namespace mad
