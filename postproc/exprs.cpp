#include<postproc/exprs.h>
#include<postproc/context.h>
#include<mfisoft/ascii_string.h>

namespace postproc {

//----------------------------------------------------------------------------
field::field(const std::string &n) : name(n)
{
    mfi::ascii::toupper(name);
}
//----------------------------------------------------------------------------
field::~field()
{
}
//----------------------------------------------------------------------------
std::string field::value(const map &fields, const context & ) const
{
    return fields[name];
}
//----------------------------------------------------------------------------
string_literal::~string_literal()
{
}
//----------------------------------------------------------------------------
std::string string_literal::value(const map & , const context & ) const
{
    return *this;
}
//----------------------------------------------------------------------------
session_constant::~session_constant()
{
}
//----------------------------------------------------------------------------
session_constant::session_constant(const std::string &n) : name(n)
{
    mfi::ascii::toupper(name);
}
//----------------------------------------------------------------------------
std::string session_constant::value(const map & , const context &ctx) const
{
    return ctx.get_session_constant(name);
}
//----------------------------------------------------------------------------
concatenation::concatenation(
        std::unique_ptr<expression> &a1, std::unique_ptr<expression> &a2)
    : arg1(a1), arg2(a2)
{
}
//----------------------------------------------------------------------------
concatenation::~concatenation()
{
}
//----------------------------------------------------------------------------
std::string concatenation::value(
    const map &fields, const context &ctx) const
{
    return arg1->value(fields, ctx) + arg2->value(fields, ctx);
}
//----------------------------------------------------------------------------

} // namespace
