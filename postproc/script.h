#ifndef __POSTPROC_SCRIPT_H
#define __POSTPROC_SCRIPT_H

#include<mfisoft/january/list.h>
#include<postproc/defs.h>
#include<postproc/rule.h>
#include<postproc/context.h>
#include<memory>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class script : private jan::non_copyable
{
    typedef jan::list<rule> list;
    list rules;
    list::iterator tail;
public:
    script();
    ~script();

    void add_rule(std::auto_ptr<condition> & , action & );

    typedef list::const_iterator const_iterator;
    const_iterator begin() const { return rules.begin(); }
    const_iterator end() const { return rules.end(); }

    bool empty() const { return rules.empty(); }
    void clear() { rules.clear(); }

    void swap(script &o)
    {
        rules.swap(o.rules);
        std::swap(tail, o.tail);
    }

    bool process(const map & , map & , const context & ) const;
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
