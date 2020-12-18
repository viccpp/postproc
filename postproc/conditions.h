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
    equal(std::unique_ptr<expression> & , std::unique_ptr<expression> & );
    ~equal();

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class not_equal : public condition
{
    equal eq;
public:
    not_equal(std::unique_ptr<expression> &a1, std::unique_ptr<expression> &a2)
        : eq(a1, a2) {}

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class in_set : public condition
{
    argument str;
    list_argument list;
public:
    in_set(std::unique_ptr<expression> & , std::unique_ptr<string_list> & );
    ~in_set();

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class not_in_set : public condition
{
    argument str;
    list_argument list;
public:
    not_in_set(std::unique_ptr<expression> & , std::unique_ptr<string_list> & );
    ~not_in_set();

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class match_regexp : public condition
{
    argument str;
    regexp re;
public:
    match_regexp(std::unique_ptr<expression> & , regexp );
    ~match_regexp();

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class not_match_regexp : public condition
{
    match_regexp cond;
public:
    not_match_regexp(std::unique_ptr<expression> &s, regexp r)
        : cond(s, std::move(r)) {}

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class cond_and : public condition
{
    not_null_ptr<const condition> cond1, cond2;
public:
    cond_and(std::unique_ptr<condition> & , std::unique_ptr<condition> & );
    ~cond_and();

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class cond_or : public condition
{
    not_null_ptr<const condition> cond1, cond2;
public:
    cond_or(std::unique_ptr<condition> & , std::unique_ptr<condition> & );
    ~cond_or();

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class cond_not : public condition
{
    not_null_ptr<const condition> cond;
public:
    explicit cond_not(std::unique_ptr<condition> & );
    ~cond_not();

    bool eval(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
