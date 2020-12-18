#ifndef POSTPROC_STRING_LIST_H
#define POSTPROC_STRING_LIST_H

#include<postproc/defs.h>
#include<postproc/exprs.h>
#include<vector>
#include<list>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class string_list
{
public:
    typedef std::vector<string_literal> values_t;
    typedef values_t::const_iterator const_iterator;

    virtual ~string_list() = default;

    virtual const values_t &get_values(const context & ) const = 0;
};
//////////////////////////////////////////////////////////////////////////////
class list_literal : public string_list, public std::vector<string_literal>
{
public:
    explicit list_literal(const std::list<string_literal> & );
    ~list_literal();

    const values_t &get_values(const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class named_list : public string_list
{
    std::string name;
public:
    explicit named_list(const std::string &n) : name(n) {}
    ~named_list();

    const values_t &get_values(const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
