#include<postproc/action.h>

namespace postproc {

//----------------------------------------------------------------------------
inline void action::free_pointers()
{
    for(const_iterator it = begin(); it != end(); ++it)
        delete *it;
}
//----------------------------------------------------------------------------
action::~action()
{
    free_pointers();
}
//----------------------------------------------------------------------------
void action::clear()
{
    free_pointers();
    ops.clear();
}
//----------------------------------------------------------------------------
action::result_type action::eval(
    const map &in_fields, map &out_fields, const context &ctx) const
{
    for(const_iterator it = begin(); it != end(); ++it)
    {
        result_type res = (*it)->eval(in_fields, out_fields, ctx);
        if(res != operation_result::nothing) return res;
    }
    return operation_result::nothing;
}
//----------------------------------------------------------------------------

} // namespace
