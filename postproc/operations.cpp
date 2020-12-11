#include<postproc/operations.h>
#include<postproc/defs.h>

namespace postproc {

//----------------------------------------------------------------------------
assignment::assignment(std::auto_ptr<field> &f, std::auto_ptr<expression> &e)
    : fld(f), expr(e)
{
}
//----------------------------------------------------------------------------
assignment::~assignment()
{
}
//----------------------------------------------------------------------------
assignment::result_type assignment::eval(
    const map &in_fields, map &out_fields, const context &ctx) const
{
    fld->set_value(expr.value(in_fields, ctx), out_fields);
    return nothing;
}
//----------------------------------------------------------------------------
swap_op::swap_op(std::auto_ptr<field> &fld1, std::auto_ptr<field> &fld2)
    : f1(fld1), f2(fld2)
{
}
//----------------------------------------------------------------------------
swap_op::~swap_op()
{
}
//----------------------------------------------------------------------------
swap_op::result_type swap_op::eval(
    const map &in_fields, map &out_fields, const context &ctx) const
{
    std::string tmp = f1->value(in_fields, ctx);
    f1->set_value(f2->value(in_fields, ctx), out_fields);
    f2->set_value(tmp, out_fields);
    return nothing;
}
//----------------------------------------------------------------------------
discard::result_type discard::eval(const map & , map & , const context & ) const
{
    return discard_record;
}
//----------------------------------------------------------------------------
break_::result_type break_::eval(const map & , map & , const context & ) const
{
    return break_script;
}
//----------------------------------------------------------------------------

} // namespace
