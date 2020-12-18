#ifndef POSTPROC_ACTION_H
#define POSTPROC_ACTION_H

#include<postproc/operation.h>
#include<vector>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class action
{
    typedef std::vector<operation*> cont_t;
    cont_t ops;
    void free_pointers();
public:
    typedef operation::result_type result_type;

    action() = default;
    action(const action &) = delete;
    action &operator=(const action &) = delete;
    ~action();

    typedef cont_t::const_iterator const_iterator;
    const_iterator begin() const { return ops.begin(); }
    const_iterator end() const { return ops.end(); }

    bool empty() const { return ops.empty(); }
    void clear();

    void push_back(operation *op) { ops.push_back(op); }
    void swap(action &o) { ops.swap(o.ops); }

    result_type eval(const map & , map & , const context & ) const;
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
