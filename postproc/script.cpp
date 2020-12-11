#include<postproc/script.h>

namespace postproc {

//----------------------------------------------------------------------------
script::script() : tail(rules.before_begin())
{
}
//----------------------------------------------------------------------------
script::~script()
{
}
//----------------------------------------------------------------------------
void script::add_rule(std::auto_ptr<condition> &cond, action &act)
{
    list::stock stock = rules.alloc_stock();
    new(stock.get_memory()) list::value_type(cond, act);
    tail = rules.insert_after(tail, stock);
}
//----------------------------------------------------------------------------
bool script::process(
    const map &in_fields, map &out_fields, const context &ctx) const
{
    for(const_iterator it = begin(); it != end(); ++it)
    {
        switch(it->eval(in_fields, out_fields, ctx))
        {
            case break_script:   return true;
            case discard_record: return false;
        }
        // TODO: copy out_fields to input ??
    }
    return true;
}
//----------------------------------------------------------------------------

} // namespace
