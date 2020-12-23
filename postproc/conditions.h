#ifndef POSTPROC_CONDITIONS_H
#define POSTPROC_CONDITIONS_H

#include<postproc/defs.h>
#include<postproc/condition.h>
#include<postproc/string_list.h>
#include<postproc/regexp.h>
#include<postproc/args.h>
#include<vector>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
// Conditions
//////////////////////////////////////////////////////////////////////////////
class equal : public condition
{
    argument arg1, arg2;
public:
    equal(unique_ptr<expression> , unique_ptr<expression> );
    ~equal();

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class not_equal : public condition
{
    equal eq;
public:
    not_equal(unique_ptr<expression> a1, unique_ptr<expression> a2)
        : eq(std::move(a1), std::move(a2)) {}
    ~not_equal();

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class in_set : public condition
{
    argument str;
    list_argument list;
public:
    in_set(unique_ptr<expression> , unique_ptr<string_list> );
    ~in_set();

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class not_in_set : public condition
{
    in_set in;
public:
    not_in_set(unique_ptr<expression> s, unique_ptr<string_list> set)
        : in(std::move(s), std::move(set)) {}
    ~not_in_set();

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class match_regexp : public condition
{
    argument str;
    regexp re;
public:
    match_regexp(unique_ptr<expression> , regexp );
    ~match_regexp();

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class not_match_regexp : public condition
{
    match_regexp cond;
public:
    not_match_regexp(unique_ptr<expression> s, regexp r)
        : cond(std::move(s), std::move(r)) {}

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class cond_and : public condition
{
    not_null_ptr<const condition> cond1, cond2;
public:
    cond_and(unique_ptr<condition> , unique_ptr<condition> );
    ~cond_and();

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class cond_or : public condition
{
    not_null_ptr<const condition> cond1, cond2;
public:
    cond_or(unique_ptr<condition> , unique_ptr<condition> );
    ~cond_or();

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class cond_not : public condition
{
    not_null_ptr<const condition> cond;
public:
    explicit cond_not(unique_ptr<condition> );
    ~cond_not();

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
