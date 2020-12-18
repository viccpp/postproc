#ifndef POSTPROC_RULE_H
#define POSTPROC_RULE_H

#include<postproc/condition.h>
#include<postproc/action.h>
#include<utility>
#include<memory>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class rule
{
    condition *cond;
    action act;
public:
    typedef operation::result_type result_type;

    rule(std::unique_ptr<condition> & , action & );
    rule(const rule &) = delete;
    rule &operator=(const rule &) = delete;
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
