#include<postproc/conditions.h>

namespace postproc {

//----------------------------------------------------------------------------
equal::equal(std::unique_ptr<expression> &a1, std::unique_ptr<expression> &a2)
    : arg1(a1), arg2(a2)
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
bool not_equal::eval(const map &fields, const context &c) const
{
    return !eq.eval(fields, c);
}
//----------------------------------------------------------------------------
in_set::in_set(std::unique_ptr<expression> &s, std::unique_ptr<string_list> &set)
    : str(s), list(set)
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
    const string_list::values_t &set = list.get_values(ctx);
    for(string_list::const_iterator it = set.begin(); it != set.end(); ++it)
        if(*it == fval) return true;
    return false;
}
//----------------------------------------------------------------------------
not_in_set::not_in_set(std::unique_ptr<expression> &s,
                                        std::unique_ptr<string_list> &set)
    : str(s), list(set)
{
}
//----------------------------------------------------------------------------
not_in_set::~not_in_set()
{
}
//----------------------------------------------------------------------------
bool not_in_set::eval(const map &fields, const context &ctx) const
{
    std::string fval = str.value(fields, ctx);
    const string_list::values_t &set = list.get_values(ctx);
    for(string_list::const_iterator it = set.begin(); it != set.end(); ++it)
        if(*it == fval) return false;
    return true;
}
//----------------------------------------------------------------------------
match_regexp::match_regexp(std::unique_ptr<expression> &s, regexp r)
    : str(s), re(std::move(r))
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
cond_and::cond_and(std::unique_ptr<condition> &c1, std::unique_ptr<condition> &c2)
    : cond1(c1), cond2(c2)
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
cond_or::cond_or(std::unique_ptr<condition> &c1, std::unique_ptr<condition> &c2)
    : cond1(c1), cond2(c2)
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
cond_not::cond_not(std::unique_ptr<condition> &c) : cond(c)
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
