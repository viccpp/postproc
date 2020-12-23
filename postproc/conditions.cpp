#include<postproc/conditions.h>

namespace postproc {

//----------------------------------------------------------------------------
equal::equal(unique_ptr<expression> a1, unique_ptr<expression> a2)
    : arg1(std::move(a1)), arg2(std::move(a2))
{
}
//----------------------------------------------------------------------------
equal::~equal()
{
}
//----------------------------------------------------------------------------
bool equal::eval(const map &fields, const context &ctx) const
{
    return arg1.value(fields, ctx) == arg2.value(fields, ctx);
}
//----------------------------------------------------------------------------
not_equal::~not_equal()
{
}
//----------------------------------------------------------------------------
bool not_equal::eval(const map &fields, const context &c) const
{
    return !eq.eval(fields, c);
}
//----------------------------------------------------------------------------
in_set::in_set(unique_ptr<expression> s, unique_ptr<string_list> set)
    : str(std::move(s)), list(std::move(set))
{
}
//----------------------------------------------------------------------------
in_set::~in_set()
{
}
//----------------------------------------------------------------------------
bool in_set::eval(const map &fields, const context &ctx) const
{
    std::string fval = str.value(fields, ctx);
    for(auto &s : list.get_values(ctx))
        if(s == fval) return true;
    return false;
}
//----------------------------------------------------------------------------
not_in_set::~not_in_set()
{
}
//----------------------------------------------------------------------------
bool not_in_set::eval(const map &fields, const context &ctx) const
{
    return !in.eval(fields, ctx);
}
//----------------------------------------------------------------------------
match_regexp::match_regexp(unique_ptr<expression> s, regexp r)
    : str(std::move(s)), re(std::move(r))
{
}
//----------------------------------------------------------------------------
match_regexp::~match_regexp()
{
}
//----------------------------------------------------------------------------
bool match_regexp::eval(const map &fields, const context &ctx) const
{
    return re.match(str.value(fields, ctx));
}
//----------------------------------------------------------------------------
bool not_match_regexp::eval(const map &fields,
                                const context &ctx) const
{
    return !cond.eval(fields, ctx);
}
//----------------------------------------------------------------------------
cond_and::cond_and(unique_ptr<condition> c1, unique_ptr<condition> c2)
    : cond1(std::move(c1)), cond2(std::move(c2))
{
}
//----------------------------------------------------------------------------
cond_and::~cond_and()
{
}
//----------------------------------------------------------------------------
bool cond_and::eval(const map &fields, const context &ctx) const
{
    return cond1->eval(fields, ctx) && cond2->eval(fields, ctx);
}
//----------------------------------------------------------------------------
cond_or::cond_or(unique_ptr<condition> c1, unique_ptr<condition> c2)
    : cond1(std::move(c1)), cond2(std::move(c2))
{
}
//----------------------------------------------------------------------------
cond_or::~cond_or()
{
}
//----------------------------------------------------------------------------
bool cond_or::eval(const map &fields, const context &ctx) const
{
    return cond1->eval(fields, ctx) || cond2->eval(fields, ctx);
}
//----------------------------------------------------------------------------
cond_not::cond_not(unique_ptr<condition> c) : cond(std::move(c))
{
}
//----------------------------------------------------------------------------
cond_not::~cond_not()
{
}
//----------------------------------------------------------------------------
bool cond_not::eval(const map &fields, const context &ctx) const
{
    return !cond->eval(fields, ctx);
}
//----------------------------------------------------------------------------

} // namespace
