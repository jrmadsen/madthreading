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


#ifndef task_tree_hh_
#define task_tree_hh_

//----------------------------------------------------------------------------//

#include "madthreading/types.hh"
#include "madthreading/threading/mutex.hh"
#include "madthreading/threading/task/task.hh"

#include <queue>
#include <stack>
#include <vector>

namespace mad
{

class task_group;

//============================================================================//
//      base class for task_tree
//============================================================================//
template <typename _Tp, typename _Arg1, typename _Arg2,
          typename _Tp_join = _Tp>
class task_tree_node : public vtask
{
public:
    typedef task_tree_node<_Tp, _Arg1, _Arg2>    this_type;
    typedef task<_Tp, _Arg1, _Arg2>              task_type;
    typedef _Tp                                  result_type;
    FUNCTION_TYPEDEF_1(join_function_type, void, _Tp_join);
    friend class task<_Tp, _Arg1, _Arg2>;

public:
    task_tree_node(task_group* tg,
                   join_function_type f,
                   task_type* task = 0,
                   _Tp val = _Tp(),
                   this_type* _parent = 0)
    : vtask(tg),
      join_function(f),
      m_parent(_parent),
      m_left_child(0), m_right_child(0),
      m_task(task), m_value(val)
    {
        m_task->set_result(&m_value);
        //m_force_delete = true;
        m_is_stored_elsewhere = true;
    }

    virtual ~task_tree_node()
    { }

public:
    _inline_
    void operator()()
    {
        (*m_task)();
        join_function(m_value);
    }

public:
    inline this_type* parent() { return m_parent; }
    inline const this_type* parent() const { return m_parent; }

    inline void parent(this_type* val) { m_parent = val; }

    inline this_type* get_task() { return this; }
    inline const this_type* get_task() const { return this; }

    inline _Tp& value() { return m_value; }
    inline const _Tp& value() const { return m_value; }

public:
    inline task_tree_node* left() const { return m_left_child; }
    inline task_tree_node* right() const { return m_right_child; }
    inline task_tree_node*& left() { return m_left_child; }
    inline task_tree_node*& right() { return m_right_child; }

protected:
    join_function_type  join_function;
    task_tree_node*     m_parent;
    task_tree_node*     m_left_child;
    task_tree_node*     m_right_child;
    task_type*          m_task;
    _Tp                 m_value;
};


//============================================================================//
//      controller class for task_tree_node
//============================================================================//
template <typename _Tree_Node_Type,
          typename _OpL = std::less<_Tree_Node_Type*>,
          typename _OpR = std::greater<_Tree_Node_Type*> >
class task_tree
{
public:
    typedef _Tree_Node_Type                         tree_node_type;
    typedef typename tree_node_type::result_type    _Tp;
    typedef typename tree_node_type::task_type      task_type;
    typedef _OpL                                    left_operator_type;
    typedef _OpR                                    right_operator_type;
    typedef std::queue<tree_node_type*>             width_list_type;
    typedef std::stack<tree_node_type*>             depth_list_type;

public:
    task_tree()
    : m_root(NULL)
    { }

    task_tree(tree_node_type* val)
    : m_root(val)
    { }

    virtual ~task_tree()
    { }

public:
    inline const tree_node_type*& root() const  { return m_root; }
    inline       tree_node_type*& root()        { return m_root; }

    inline void set_root(tree_node_type* val) { m_root = val; }
    inline void insert(tree_node_type*&, tree_node_type*, tree_node_type* = 0);

    inline width_list_type width_traverse(tree_node_type*);
    inline depth_list_type depth_traverse(tree_node_type*);

    template <typename _Func>
    inline void operator()(tree_node_type*, _Func);
    template <typename _Func>
    inline _Tp operator()(tree_node_type*, _Func, _Tp);
    template <typename _Func>
    inline void width_traverse(tree_node_type*, _Func);
    template <typename _Func>
    inline void depth_traverse(tree_node_type*, _Func);

    inline bool search(_Tp, tree_node_type*);

private:
    tree_node_type* m_root;
    left_operator_type _left_operator;
    right_operator_type _right_operator;
};

//============================================================================//
template <typename _TN, typename _OpL, typename _OpR>
inline void task_tree<_TN, _OpL, _OpR>::insert(tree_node_type*& subroot,
                                               tree_node_type* tn,
                                               tree_node_type* parent)
{
    if(!subroot)
    {
        subroot = tn;
        subroot->parent(parent);
    }

    if(_left_operator(tn, subroot))
        insert(subroot->left(), tn, subroot);

    else if(_right_operator(tn, subroot))
        insert(subroot->right(), tn, subroot);
}
//============================================================================//
template <typename _TN, typename _OpL, typename _OpR>
template <typename _Func>
inline typename task_tree<_TN, _OpL, _OpR>::_Tp
task_tree<_TN, _OpL, _OpR>::operator()(tree_node_type* subroot,
                                       _Func func,
                                       _Tp identity)
{
    if(subroot->left())
        (*this)(subroot->left(), func, identity);

    func(identity, subroot->value());

    if(subroot->right())
        (*this)(subroot->right(), func, identity);

    return identity;
}
//============================================================================//
template <typename _TN, typename _OpL, typename _OpR>
template <typename _Func>
inline void
task_tree<_TN, _OpL, _OpR>::operator()(tree_node_type* subroot,
                                       _Func func)
{
    if(subroot->left())
        (*this)(subroot->left(), func);

    func(subroot);

    if(subroot->right())
        (*this)(subroot->right(), func);

}
//============================================================================//
template <typename _TN, typename _OpL, typename _OpR>
inline typename task_tree<_TN, _OpL, _OpR>::width_list_type
task_tree<_TN, _OpL, _OpR>::width_traverse(tree_node_type* subroot)
{
    width_list_type q;
    width_list_type r;
    if(!subroot)
        return q;

    q.push(subroot);
    while(!q.empty())
    {
        tree_node_type* tmp = q.front();
        q.pop();
        if(tmp)
            r.push(tmp);
        if(tmp->left())
            q.push(tmp->left());
        if(tmp->right())
            q.push(tmp->right());
    }
    return r;
}
//============================================================================//
template <typename _TN, typename _OpL, typename _OpR>
inline typename task_tree<_TN, _OpL, _OpR>::depth_list_type
task_tree<_TN, _OpL, _OpR>::depth_traverse(tree_node_type* subroot)
{
    depth_list_type s;
    depth_list_type r;

    if(!subroot)
        return r;

    s.push(subroot);
    while(!s.empty())
    {
        tree_node_type* tmp = s.top();
        s.pop();
        if(tmp)
            r.push(tmp);
        if(tmp->right())
            s.push(tmp->right());
        if(tmp->left())
            s.push(tmp->left());
    }
    return r;
}
//============================================================================//
template <typename _TN, typename _OpL, typename _OpR>
template <typename _Func>
inline void task_tree<_TN, _OpL, _OpR>::width_traverse(tree_node_type* subroot,
                                                       _Func func)
{
    std::queue<tree_node_type*> q;
    if(!subroot)
        return;

    q.push(*subroot);
    while(!q.empty())
    {
        tree_node_type* tmp = q.front();
        func(tmp->value());
        if(tmp->left())
            q.push(tmp->left());
        if(tmp->right())
            q.push(tmp->right());
    }
}
//============================================================================//
template <typename _TN, typename _OpL, typename _OpR>
template <typename _Func>
inline void task_tree<_TN, _OpL, _OpR>::depth_traverse(tree_node_type* subroot,
                                                       _Func func)
{
    std::stack<tree_node_type*> s;
    if(!subroot)
        return;

    s.push(subroot);
    while(!s.empty())
    {
        tree_node_type* tmp = s.top();
        func(tmp->value());
        if(tmp->right())
            s.push(tmp->right());
        if(tmp->left())
            s.push(tmp->left());
    }
}
//============================================================================//
template <typename _TN, typename _OpL, typename _OpR>
inline bool task_tree<_TN, _OpL, _OpR>::search(_Tp val,
                                               tree_node_type* subroot)
{
    tree_node_type* current = subroot;
    while(current)
    {
        if(_left_operator(val, current->value()))
            current = current->left();
        else if(_right_operator(val, current->value()))
            current = current->right();
        else
            return true;
    }
    return false;
}
//============================================================================//

} // namespace mad

#endif
