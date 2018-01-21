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


#ifndef allocator_list_hh_
#define allocator_list_hh_

#include <deque>
#include <vector>
#include "madthreading/threading/tls.hh"

#define ThreadGlobal

namespace mad
{
namespace details
{

class allocator_base;

//============================================================================//
// thread-global version
class allocator_list
{
public:
    typedef std::size_t size_type;
    typedef std::vector<allocator_base*> list_type;

public:
    // public static functions
    static allocator_list* get_allocator_list();
    static allocator_list* get_allocator_list_if_exists();

public:
    // Virtual destructors are required by abstract classes
    // so add it by default, just in case
    ~allocator_list();

public:
    // Public functions
    void Register(allocator_base*);
    void Destroy(size_type nstat = 0, int verbose = 0);
    size_type size() const;

private:
    // Private functions
    // Constructor and Destructors
    allocator_list();

private:
    // Private variables
    static allocator_list* m_allocator_list;
    list_type m_list;
};

//============================================================================//
// thread-local version
class allocator_list_tl
{
public:
    typedef std::size_t size_type;
    typedef std::vector<allocator_base*> list_type;

public:
    // public static functions
    static allocator_list_tl* get_allocator_list();
    static allocator_list_tl* get_allocator_list_if_exists();

public:
    // Virtual destructors are required by abstract classes
    // so add it by default, just in case
    ~allocator_list_tl();

public:
    // Public functions
    void Register(allocator_base*);
    void Destroy(size_type nstat = 0, int verbose = 0);
    size_type size() const;

private:
    // Private functions
    // Constructor and Destructors
    allocator_list_tl();

private:
    // Private variables
    ThreadLocalStatic allocator_list_tl* m_allocator_list_tl;
    list_type m_list;


};

//============================================================================//

} // namespace details
} // namespace mad

#endif
