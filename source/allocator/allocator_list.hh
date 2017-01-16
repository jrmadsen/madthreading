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


#ifndef allocator_list_hh_
#define allocator_list_hh_

//----------------------------------------------------------------------------//
#ifdef SWIG
%module allocator_list
%{
    #include "tls.hh"
    #include "allocator_list.hh"
%}

#ifdef ThreadLocal
#   undef ThreadLocal
#endif
#ifdef ThreadLocalStatic
#   undef ThreadLocalStatic
#endif
#define ThreadLocal thread_local
#define ThreadLocalStatic static thread_local

%include "allocator_list.hh"

#endif
//----------------------------------------------------------------------------//

#include <deque>
#include <vector>
#include "../threading/tls.hh"

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
