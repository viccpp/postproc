#include<postproc/script.h>

namespace postproc {

//----------------------------------------------------------------------------
script::script() : tail(rules.before_begin())
{
}
//----------------------------------------------------------------------------
script::~script()
{
}
//----------------------------------------------------------------------------
void script::add_rule(std::unique_ptr<condition> &cond, action &act)
{
    tail = rules.emplace_after(tail, cond, act);
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
        }
        // TODO: copy out_fields to input ??
    }
    return true;
}
//----------------------------------------------------------------------------

} // namespace
