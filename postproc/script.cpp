#include<postproc/script.h>

namespace postproc {

//----------------------------------------------------------------------------
script::script()
{
}
//----------------------------------------------------------------------------
script::~script()
{
}
//----------------------------------------------------------------------------
void script::add_rule(unique_ptr<condition> cond, action act)
{
    tail = rules.emplace_after(tail, std::move(cond), std::move(act));
}
//----------------------------------------------------------------------------
bool script::process(
    const map &in_fields, map &out_fields, const context &ctx) const
{
    for(auto &rule : rules)
    {
        switch(rule.eval(in_fields, out_fields, ctx))
        {
            case operation_result::break_script:   return true;
            case operation_result::discard_record: return false;
            default:; // eval next
        }
        // TODO: copy out_fields to input ??
    }
    return true;
}
//----------------------------------------------------------------------------

} // namespace
