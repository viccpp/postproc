#include<postproc/predicates.h>

namespace postproc { namespace predicates {

//----------------------------------------------------------------------------
empty::empty(std::unique_ptr<expression> &str) : arg(str)
{
}
//----------------------------------------------------------------------------
empty::~empty()
{
}
//----------------------------------------------------------------------------
bool empty::eval(const map &fields, const context &ctx) const
{
    return arg.value(fields, ctx).empty();
}
//----------------------------------------------------------------------------
starts_with::starts_with(
            std::unique_ptr<expression> &str, std::unique_ptr<expression> &pref)
    : arg1(str), arg2(pref)
{
}
//----------------------------------------------------------------------------
starts_with::~starts_with()
{
}
//----------------------------------------------------------------------------
bool starts_with::eval(const map &fields, const context &ctx) const
{
    std::string str  = arg1.value(fields, ctx),
                pref = arg2.value(fields, ctx);
    return str.length() >= pref.length() &&
            str.compare(0, pref.length(), pref) == 0;
}
//----------------------------------------------------------------------------
const char lt::func[] = "lt";
lt::lt(std::unique_ptr<expression> &n1, std::unique_ptr<expression> &n2)
    : arg1(n1), arg2(n2)
{
}
//----------------------------------------------------------------------------
lt::~lt()
{
}
//----------------------------------------------------------------------------
bool lt::eval(const map &fields, const context &ctx) const
{
    return arg1.value(fields, ctx) < arg2.value(fields, ctx);
}
//----------------------------------------------------------------------------
const char gt::func[] = "gt";
gt::gt(std::unique_ptr<expression> &n1, std::unique_ptr<expression> &n2)
    : arg1(n1), arg2(n2)
{
}
//----------------------------------------------------------------------------
gt::~gt()
{
}
//----------------------------------------------------------------------------
bool gt::eval(const map &fields, const context &ctx) const
{
    return arg1.value(fields, ctx) > arg2.value(fields, ctx);
}
//----------------------------------------------------------------------------
const char le::func[] = "le";
le::le(std::unique_ptr<expression> &n1, std::unique_ptr<expression> &n2)
    : arg1(n1), arg2(n2)
{
}
//----------------------------------------------------------------------------
le::~le()
{
}
//----------------------------------------------------------------------------
bool le::eval(const map &fields, const context &ctx) const
{
    return arg1.value(fields, ctx) <= arg2.value(fields, ctx);
}
//----------------------------------------------------------------------------
const char ge::func[] = "ge";
ge::ge(std::unique_ptr<expression> &n1, std::unique_ptr<expression> &n2)
    : arg1(n1), arg2(n2)
{
}
//----------------------------------------------------------------------------
ge::~ge()
{
}
//----------------------------------------------------------------------------
bool ge::eval(const map &fields, const context &ctx) const
{
    return arg1.value(fields, ctx) >= arg2.value(fields, ctx);
}
//----------------------------------------------------------------------------

}} // namespace
