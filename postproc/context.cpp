#include<postproc/context.h>

namespace postproc {

const context::stub_const_extractor context::stub_sce{};
//----------------------------------------------------------------------------
context::context() : sce(&stub_sce)
{
}
//----------------------------------------------------------------------------
context::context(const session_constant_extractor &ex)
    : sce(&ex)
{
}
//----------------------------------------------------------------------------
context::~context()
{
}
//----------------------------------------------------------------------------
const std::vector<string_literal> *context::get_named_list(
    const std::string &name) const
{
    named_lists_map::const_iterator it = named_lists.find(name);
    return it == named_lists.end() ? nullptr : &it->second;
}
//----------------------------------------------------------------------------
void context::add_list(
    const std::string &name, const std::list<string_literal> &list)
{
    std::vector<string_literal> &el = named_lists[name];
    el.reserve(list.size());
    el.assign(list.begin(), list.end());
}
//----------------------------------------------------------------------------

} // namespace
