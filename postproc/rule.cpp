#include<postproc/rule.h>

namespace postproc {

//----------------------------------------------------------------------------
rule::rule(unique_ptr<condition> c, action a)
    : cond(std::move(c)), act(std::move(a))
{
}
//----------------------------------------------------------------------------
rule::result_type rule::eval(
    const map &in_fields, map &out_fields, const context &ctx) const
{
    if(!cond || cond->eval(in_fields, ctx))
        return act.eval(in_fields, out_fields, ctx);
    return operation_result::nothing;
}
//----------------------------------------------------------------------------

} // namespace
