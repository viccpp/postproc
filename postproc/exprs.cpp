#include<postproc/exprs.h>
#include<postproc/context.h>
#include<__vic/ascii_string.h>

namespace postproc {

//----------------------------------------------------------------------------
field::field(std::string n) : name(std::move(n))
{
    __vic::ascii::toupper(name);
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
session_constant::session_constant(std::string n) : name(std::move(n))
{
    __vic::ascii::toupper(name);
}
//----------------------------------------------------------------------------
std::string session_constant::value(const map & , const context &ctx) const
{
    return ctx.get_session_constant(name);
}
//----------------------------------------------------------------------------
concatenation::concatenation(
    unique_ptr<expression> a1, unique_ptr<expression> a2)
:
    arg1(std::move(a1)), arg2(std::move(a2))
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
