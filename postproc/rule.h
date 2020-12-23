#ifndef POSTPROC_RULE_H
#define POSTPROC_RULE_H

#include<postproc/condition.h>
#include<postproc/action.h>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class rule
{
    unique_ptr<condition> cond;
    action act;
public:
    typedef operation::result_type result_type;

    rule(unique_ptr<condition> , action );

    void swap(rule &o)
    {
        cond.swap(o.cond);
        act.swap(o.act);
    }

    result_type eval(const map & , map & , const context & ) const;
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
