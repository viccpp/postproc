#include<postproc/string_list.h>
#include<postproc/context.h>

namespace postproc {

//----------------------------------------------------------------------------
list_literal::list_literal(const std::list<string_literal> &set)
{
    this->reserve(set.size());
    this->assign(set.begin(), set.end());
}
//----------------------------------------------------------------------------
list_literal::~list_literal()
{
}
//----------------------------------------------------------------------------
const string_list::values_t &list_literal::get_values(const context & ) const
{
    return *this;
}
//----------------------------------------------------------------------------
named_list::~named_list()
{
}
//----------------------------------------------------------------------------
const string_list::values_t &named_list::get_values(const context &ctx) const
{
    const values_t *p = ctx.get_named_list(name);
    if(!p) throw unknown_named_list(name);
    return *p;
}
//----------------------------------------------------------------------------

} // namespace
