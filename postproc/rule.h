#ifndef __POSTPROC_RULE_H
#define __POSTPROC_RULE_H

#include<mfisoft/january/defs.h>
#include<postproc/condition.h>
#include<postproc/action.h>
#include<memory>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class rule : private jan::non_copyable
{
    condition *cond;
    action act;
public:
    typedef operation::result_type result_type;

    rule(std::auto_ptr<condition> & , action & );
    ~rule();

    void swap(rule &o)
    {
        std::swap(cond, o.cond);
        act.swap(o.act);
    }

    result_type eval(const map & , map & , const context & ) const;
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
