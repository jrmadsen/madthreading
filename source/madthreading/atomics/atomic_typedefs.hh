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
// created by jrmadsen on Tue Jun 10 14:43:08 2014
//
//
//
//

#ifndef atomic_typedefs_hh_
#define atomic_typedefs_hh_

//============================================================================//

#include <functional>
#include <atomic>
#include "../threading/threading.hh"
#include "../macros.hh"

namespace base_atomic = std;

//============================================================================//

namespace mad
{

namespace atomics
{
    //------------------------------------------------------------------------//
    namespace details
    {
        //--------------------------------------------------------------------//
        // The following structs are used for the operator's in
        //--------------------------------------------------------------------//
        template <typename _Tp>
        struct base_binary : public std::binary_function<_Tp, _Tp, _Tp>
        {
            virtual ~base_binary() { }

            typedef typename std::binary_function<_Tp, _Tp, _Tp>
            ::result_type result_type;
            typedef typename std::binary_function<_Tp, _Tp, _Tp>
            ::first_argument_type first_argument_type;
            typedef typename std::binary_function<_Tp, _Tp, _Tp>
            ::second_argument_type second_argument_type;

            virtual
            result_type operator() (first_argument_type,
                                    second_argument_type) const = 0;
        };
        //--------------------------------------------------------------------//
        template <typename _Tp>
        struct set1st : public base_binary<_Tp>
        {
            typedef typename base_binary<_Tp>
            ::result_type result_type;
            typedef typename base_binary<_Tp>
            ::first_argument_type first_argument_type;
            typedef typename base_binary<_Tp>
            ::second_argument_type second_argument_type;

            _inline_
            result_type operator()(first_argument_type _val1,
                                   second_argument_type) const
            { return _val1; }
        };
        //--------------------------------------------------------------------//
        template <typename _Tp>
        struct set2nd : public base_binary<_Tp>
        {
            typedef typename base_binary<_Tp>
            ::result_type result_type;
            typedef typename base_binary<_Tp>
            ::first_argument_type first_argument_type;
            typedef typename base_binary<_Tp>
            ::second_argument_type second_argument_type;

            _inline_
            result_type operator()(first_argument_type,
                                   second_argument_type _val2) const
            { return _val2; }
        };
        //--------------------------------------------------------------------//
        template <typename _Tp>
        struct plus : public base_binary<_Tp>
        {
            typedef typename base_binary<_Tp>
            ::result_type result_type;
            typedef typename base_binary<_Tp>
            ::first_argument_type first_argument_type;
            typedef typename base_binary<_Tp>
            ::second_argument_type second_argument_type;

            _inline_
            result_type operator()(first_argument_type _val1,
                                   second_argument_type _val2) const
            { return _val1 + _val2; }
        };
        //--------------------------------------------------------------------//
        template <typename _Tp>
        struct minus : public base_binary<_Tp>
        {
            typedef typename base_binary<_Tp>
            ::result_type result_type;
            typedef typename base_binary<_Tp>
            ::first_argument_type first_argument_type;
            typedef typename base_binary<_Tp>
            ::second_argument_type second_argument_type;

            _inline_
            result_type operator()(first_argument_type _val1,
                                   second_argument_type _val2) const
            { return _val1 - _val2; }
        };
        //--------------------------------------------------------------------//
        template <typename _Tp>
        struct multiplies : public base_binary<_Tp>
        {
            typedef typename base_binary<_Tp>
            ::result_type result_type;
            typedef typename base_binary<_Tp>
            ::first_argument_type first_argument_type;
            typedef typename base_binary<_Tp>
            ::second_argument_type second_argument_type;

            _inline_
            result_type operator()(first_argument_type _val1,
                                   second_argument_type _val2) const
            { return _val1 * _val2; }
        };
        //--------------------------------------------------------------------//
        template <typename _Tp>
        struct divides : public base_binary<_Tp>
        {
            typedef typename base_binary<_Tp>
            ::result_type result_type;
            typedef typename base_binary<_Tp>
            ::first_argument_type first_argument_type;
            typedef typename base_binary<_Tp>
            ::second_argument_type second_argument_type;

            _inline_
            result_type operator()(first_argument_type _val1,
                                   second_argument_type _val2) const
            { return _val1 / _val2; }
        };
        //--------------------------------------------------------------------//
    }
    //------------------------------------------------------------------------//

    namespace mem_order
    {
        using base_atomic::memory_order;
    }

    // MO = MemoryOrder
    template <typename _Tp>
    class MO
    {
    public:
        static base_atomic::memory_order default_mem_order;
        static base_atomic::memory_order secondary_mem_order;
        static void SetDefault(base_atomic::memory_order);
        static void SetSecondary(base_atomic::memory_order);

    };

    template <typename _Tp>
    base_atomic::memory_order MO<_Tp>::default_mem_order
    = base_atomic::memory_order_acq_rel;

    template <typename _Tp>
    base_atomic::memory_order MO<_Tp>::secondary_mem_order
    = base_atomic::memory_order_relaxed;

    template <typename _Tp>
    void MO<_Tp>::SetDefault(base_atomic::memory_order _odr)
    { default_mem_order = _odr; }

    template <typename _Tp>
    void MO<_Tp>::SetSecondary(base_atomic::memory_order _odr)
    { secondary_mem_order = _odr; }

#ifdef CXX11
    template <typename _Tp> using MemoryOrder = MO<_Tp>;
#endif

    //------------------------------------------------------------------------//
    namespace details
    {
        //--------------------------------------------------------------------//
        template <typename _Tp>
        inline void do_fetch_and_store(base_atomic::atomic<_Tp>* _atomic,
                                       const _Tp& _value,
                                       base_atomic::memory_order mem_odr
                                       = base_atomic::memory_order_seq_cst)
        {
            _atomic->store(_value, mem_odr);
        }
        //--------------------------------------------------------------------//
        template <typename _Tp>
        inline void do_fetch_and_store(base_atomic::atomic<_Tp>* _atomic,
                                       const base_atomic::atomic<_Tp>& _value,
                                       base_atomic::memory_order mem_odr
                                       = base_atomic::memory_order_seq_cst)
        {
            _atomic->store(_value.load(), mem_odr);
        }
        //--------------------------------------------------------------------//
        template <typename _Tp>
        inline void do_compare_and_swap(base_atomic::atomic<_Tp>* _atomic,
                          const _Tp& _value,
                          const base_binary<_Tp>& _operator,
                          base_atomic::memory_order mem_odr_1,
                          base_atomic::memory_order mem_odr_2)
        {
            _Tp _expected = _Tp();
            do {
                _expected = _atomic->load();
            } while (!(_atomic->compare_exchange_weak(_expected,
                                                      _operator(_expected,
                                                                _value),
                                                      mem_odr_1,
                                                      mem_odr_2)));
        }
        //--------------------------------------------------------------------//
        template <typename _Tp>
        inline void do_compare_and_swap(base_atomic::atomic<_Tp>* _atomic,
                        const base_atomic::atomic<_Tp>&
                                            _atomic_value,
                        const base_binary<_Tp>& _operator,
                        base_atomic::memory_order mem_odr_1,
                        base_atomic::memory_order mem_odr_2)
        {
            _Tp _expected = _Tp();
            do {
                _expected = _atomic->load();
            } while (!(_atomic->compare_exchange_weak(_expected,
                                                      _operator(_expected,
                                                                _atomic_value.load()),
                                                      mem_odr_1,
                                                      mem_odr_2)));
        }
        //--------------------------------------------------------------------//
    }
    //------------------------------------------------------------------------//
    //  WITH ATOMIC TEMPLATE BASE TYPE AS SECOND PARAMETER
    //------------------------------------------------------------------------//
    template <typename T>
    inline void set(base_atomic::atomic<T>* _atomic, const T& _desired,
                    base_atomic::memory_order mem_odr_1
                    = MO<T>::default_mem_order,
                    base_atomic::memory_order mem_odr_2
                    = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _desired, details::set2nd<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void set(base_atomic::atomic<T>& _atomic, const T& _desired,
                    base_atomic::memory_order mem_odr_1
                    = MO<T>::default_mem_order,
                    base_atomic::memory_order mem_odr_2
                    = MO<T>::secondary_mem_order)
    {
        set(&_atomic, _desired, mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void increment(base_atomic::atomic<T>* _atomic, const T& _increment,
                          base_atomic::memory_order mem_odr_1
                          = MO<T>::default_mem_order,
                          base_atomic::memory_order mem_odr_2
                          = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _increment, details::plus<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void decrement(base_atomic::atomic<T>* _atomic, const T& _decrement,
                          base_atomic::memory_order mem_odr_1
                          = MO<T>::default_mem_order,
                          base_atomic::memory_order mem_odr_2
                          = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _decrement, details::minus<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void multiply(base_atomic::atomic<T>* _atomic, const T& _factor,
                         base_atomic::memory_order mem_odr_1
                         = MO<T>::default_mem_order,
                         base_atomic::memory_order mem_odr_2
                         = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _factor, details::multiplies<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void divide(base_atomic::atomic<T>* _atomic, const T& _factor,
                       base_atomic::memory_order mem_odr_1
                       = MO<T>::default_mem_order,
                       base_atomic::memory_order mem_odr_2
                       = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _factor, details::divides<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    //  WITH ATOMICS AS SECOND PARAMETER
    //------------------------------------------------------------------------//
    template <typename T>
    inline void set(base_atomic::atomic<T>* _atomic,
                    const base_atomic::atomic<T>& _atomic_desired,
                    base_atomic::memory_order mem_odr_1
                    = MO<T>::default_mem_order,
                    base_atomic::memory_order mem_odr_2
                    = MO<T>::secondary_mem_order)
    {
        //details::do_fetch_and_store(_atomic, _desired);
        details::do_compare_and_swap(_atomic, _atomic_desired,
                                     details::set2nd<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void set(base_atomic::atomic<T>& _atomic,
                    const base_atomic::atomic<T>& _atomic_desired,
                    base_atomic::memory_order mem_odr_1
                    = MO<T>::default_mem_order,
                    base_atomic::memory_order mem_odr_2
                    = MO<T>::secondary_mem_order)
    {
        set(&_atomic, _atomic_desired, mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void increment(base_atomic::atomic<T>* _atomic,
                          const base_atomic::atomic<T>& _atomic_increment,
                          base_atomic::memory_order mem_odr_1
                          = MO<T>::default_mem_order,
                          base_atomic::memory_order mem_odr_2
                          = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _atomic_increment,
                                     details::plus<T>(), mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void decrement(base_atomic::atomic<T>* _atomic,
                          const base_atomic::atomic<T>& _atomic_decrement,
                          base_atomic::memory_order mem_odr_1
                          = MO<T>::default_mem_order,
                          base_atomic::memory_order mem_odr_2
                          = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _atomic_decrement,
                                     details::minus<T>(), mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void multiply(base_atomic::atomic<T>* _atomic,
                         const base_atomic::atomic<T>& _atomic_factor,
                         base_atomic::memory_order mem_odr_1
                         = MO<T>::default_mem_order,
                         base_atomic::memory_order mem_odr_2
                         = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _atomic_factor,
                                     details::multiplies<T>(),
                                     mem_odr_1, mem_odr_2);
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void divide(base_atomic::atomic<T>* _atomic,
                       const base_atomic::atomic<T>& _atomic_factor,
                       base_atomic::memory_order mem_odr_1
                       = MO<T>::default_mem_order,
                       base_atomic::memory_order mem_odr_2
                       = MO<T>::secondary_mem_order)
    {
        details::do_compare_and_swap(_atomic, _atomic_factor,
                                     details::divides<T>(),
                                     mem_odr_1, mem_odr_2);
    }

    //------------------------------------------------------------------------//
    //               STANDARD OVERLOAD                                        //
    //------------------------------------------------------------------------//
    template <typename T>
    inline void set(T* _non_atomic, const T& _desired)
    {
      *_non_atomic = _desired;
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline void set(T& _non_atomic, const T& _desired)
    {
      set(&_non_atomic, _desired);
    }
    //------------------------------------------------------------------------//
    //               STL PAIR OVERLOAD                                        //
    //------------------------------------------------------------------------//
    //
    //------------------------------------------------------------------------//
    //  WITH ATOMIC TEMPLATE TYPE AS SECOND PARAMETER
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void set(std::pair<base_atomic::atomic<T>,
                              base_atomic::atomic<U> >* _atomic,
                    const std::pair<T, U>& _desired)
    {
        set(&_atomic->first, _desired.first);
        set(&_atomic->second, _desired.second);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void set(std::pair<base_atomic::atomic<T>,
                              base_atomic::atomic<U> >& _atomic,
                    const std::pair<T, U>& _desired)
    {
        set(&_atomic, _desired);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void increment(std::pair<base_atomic::atomic<T>,
                                    base_atomic::atomic<U> >* _atomic,
                          const std::pair<T, U>& _increment)
    {
        increment(&_atomic->first, _increment.first);
        increment(&_atomic->second, _increment.second);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void decrement(std::pair<base_atomic::atomic<T>,
                                    base_atomic::atomic<U> >* _atomic,
                          const std::pair<T, U>& _decrement)
    {
        decrement(&_atomic->first, _decrement.first);
        decrement(&_atomic->second, _decrement.second);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void multiply(std::pair<base_atomic::atomic<T>,
                                   base_atomic::atomic<U> >* _atomic,
                         const std::pair<T, U>& _factor)
    {
        multiply(&_atomic->first, _factor.first);
        multiply(&_atomic->second, _factor.second);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void divide(std::pair<base_atomic::atomic<T>,
                                 base_atomic::atomic<U> >* _atomic,
                       const std::pair<T, U>& _factor)
    {
        divide(&_atomic->first, _factor.first);
        divide(&_atomic->second, _factor.second);
    }
    //------------------------------------------------------------------------//
    //  WITH ATOMICS AS SECOND PARAMETER
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void set(std::pair<base_atomic::atomic<T>,
                              base_atomic::atomic<U> >* _atomic,
                    const std::pair<base_atomic::atomic<T>,
                                    base_atomic::atomic<U> >& _desired)
    {
        set(&_atomic->first, _desired.first);
        set(&_atomic->second, _desired.second);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void set(std::pair<base_atomic::atomic<T>,
                              base_atomic::atomic<U> >& _atomic,
                    const std::pair<base_atomic::atomic<T>,
                                    base_atomic::atomic<U> >& _desired)
    {
        set(&_atomic, _desired);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void increment(std::pair<base_atomic::atomic<T>,
                                    base_atomic::atomic<U> >* _atomic,
                          const std::pair<base_atomic::atomic<T>,
                                          base_atomic::atomic<U> >& _increment)
    {
        increment(&_atomic->first, _increment.first);
        increment(&_atomic->second, _increment.second);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void decrement(std::pair<base_atomic::atomic<T>,
                                    base_atomic::atomic<U> >* _atomic,
                          const std::pair<base_atomic::atomic<T>,
                                          base_atomic::atomic<U> >& _decrement)
    {
        decrement(&_atomic->first, _decrement.first);
        decrement(&_atomic->second, _decrement.second);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void multiply(std::pair<base_atomic::atomic<T>,
                                   base_atomic::atomic<U> >* _atomic,
                         const std::pair<base_atomic::atomic<T>,
                                         base_atomic::atomic<U> >& _factor)
    {
        multiply(&_atomic->first, _factor.first);
        multiply(&_atomic->second, _factor.second);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline void divide(std::pair<base_atomic::atomic<T>,
                                 base_atomic::atomic<U> >* _atomic,
                       const std::pair<base_atomic::atomic<T>,
                                       base_atomic::atomic<U> >& _factor)
    {
        divide(&_atomic->first, _factor.first);
        divide(&_atomic->second, _factor.second);
    }
    //------------------------------------------------------------------------//


    //------------------------------------------------------------------------//
    template <typename T>
    inline T get(const T& _non_atomic)
    {
        return _non_atomic;
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline T get(const T& _non_atomic, base_atomic::memory_order)
    {
        return _non_atomic;
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline T get(const base_atomic::atomic<T>& _atomic)
    {
        return _atomic.load();
    }
    //------------------------------------------------------------------------//
    template <typename T>
    inline T get(const base_atomic::atomic<T>& _atomic,
                 base_atomic::memory_order mem_odr)
    {
        return _atomic.load(mem_odr);
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline std::pair<T, U> get(const std::pair<base_atomic::atomic<T>,
                               base_atomic::atomic<U> >& _atomic)
    {
        return std::pair<T, U>(get(_atomic.first), get(_atomic.second));
    }
    //------------------------------------------------------------------------//
    template <typename T, typename U>
    inline std::pair<T, U> get(const std::pair<base_atomic::atomic<T>,
                               base_atomic::atomic<U> >& _atomic,
                               base_atomic::memory_order mem_odr)
    {
        return std::pair<T, U>(get(_atomic.first, mem_odr),
                               get(_atomic.second, mem_odr));
    }
    //------------------------------------------------------------------------//



    //------------------------------------------------------------------------//
    // for plain old data (POD) and pairs (e.g. std::pair<atomic<T>, atomic<U> >)
    template <typename _Tp_base, typename _Tp_atom>
    inline _Tp_base base(const _Tp_atom& _atomic)
    {
        return get(_atomic);
    }
    //------------------------------------------------------------------------//

}

//============================================================================//

} // namespace mad

//============================================================================//

#endif  // atomic_typedefs_hh_
