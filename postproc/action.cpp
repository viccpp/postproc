#include<postproc/action.h>

namespace postproc {

//----------------------------------------------------------------------------
action::result_type action::eval(
    const map &in_fields, map &out_fields, const context &ctx) const
{
    for(auto &op : ops)
    {
        result_type res = op->eval(in_fields, out_fields, ctx);
        if(res != operation_result::nothing) return res;
    }
    return operation_result::nothing;
}
//----------------------------------------------------------------------------

} // namespace
