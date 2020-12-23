#ifndef POSTPROC_ACTION_H
#define POSTPROC_ACTION_H

#include<postproc/operation.h>
#include<postproc/smart_ptr.h>
#include<initializer_list>
#include<vector>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class action
{
    std::vector<unique_ptr<operation>> ops;
public:
    typedef operation::result_type result_type;

    template<class... OpPtr>
    action(OpPtr&&... op)
    {
        (void) std::initializer_list<char>{
            (push_back(std::move(op)), '\0')...
        };
    }

    auto begin() const { return ops.begin(); }
    auto end() const { return ops.end(); }

    bool empty() const { return ops.empty(); }
    void clear() { ops.clear(); }

    void push_back(unique_ptr<operation> op) { ops.push_back(std::move(op)); }
    void swap(action &o) { ops.swap(o.ops); }

    result_type eval(const map & , map & , const context & ) const;
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
