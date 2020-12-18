#ifndef POSTPROC_SCRIPT_H
#define POSTPROC_SCRIPT_H

#include<postproc/rule.h>
#include<postproc/context.h>
#include<forward_list>
#include<utility>
#include<memory>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class script
{
    std::forward_list<rule> rules;
    decltype(rules)::iterator tail;
public:
    script();
    script(const script &) = delete;
    script &operator=(const script &) = delete;
    ~script();

    void add_rule(std::unique_ptr<condition> & , action & );

    auto begin() const { return rules.begin(); }
    auto end() const { return rules.end(); }

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
