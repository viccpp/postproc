#ifndef POSTPROC_SCRIPT_H
#define POSTPROC_SCRIPT_H

#include<postproc/rule.h>
#include<postproc/context.h>
#include<forward_list>
#include<utility>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class script
{
    std::forward_list<rule> rules;
    decltype(rules)::iterator tail = rules.before_begin();
    void reset_tail() { tail = rules.before_begin(); }
public:
    script();
    script(script &&o) noexcept
        : rules(std::move(o.rules)), tail(o.tail) { o.reset_tail(); }
    script(const script &) = delete;
    ~script();

    script &operator=(script &&o) noexcept { swap(o); return *this; }
    script &operator=(const script &) = delete;

    void add_rule(unique_ptr<condition> , action );

    auto begin() const { return rules.begin(); }
    auto end() const { return rules.end(); }

    bool empty() const { return rules.empty(); }
    void clear() { rules.clear(); reset_tail(); }

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
