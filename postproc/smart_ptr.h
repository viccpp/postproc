#ifndef __POSTPROC_SMART_PTR_H
#define __POSTPROC_SMART_PTR_H

#include<mfisoft/january/defs.h>
#include<postproc/defs.h>
#include<memory>
#include<cassert>

namespace postproc {

enum not_null_t { not_null }; // constructor tag
//////////////////////////////////////////////////////////////////////////////
// Never NULL non-copyable auto_ptr-like class
template<class T>
class not_null_ptr : private jan::non_copyable
{
    T *p;
public:
    template<class Q>
    explicit not_null_ptr(std::auto_ptr<Q> &ptr) : p(ptr.get())
        { if(!p) throw null_pointer(); ptr.release(); }
    // Constructor w/o check!
    not_null_ptr(not_null_t, T *ptr) : p(ptr) { assert(ptr); }
    ~not_null_ptr() { delete p; }
    T *operator->() { return p; }
    const T *operator->() const { return p; }
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
