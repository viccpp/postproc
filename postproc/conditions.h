#ifndef __POSTPROC_CONDITIONS_H
#define __POSTPROC_CONDITIONS_H

#include<postproc/defs.h>
#include<postproc/condition.h>
#include<postproc/string_list.h>
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
    equal(std::auto_ptr<expression> & , std::auto_ptr<expression> & );
    ~equal();

    bool eval(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
class not_equal : public condition
{
    equal eq;
public:
    not_equal(std::auto_ptr<expression> &a1, std::auto_ptr<expression> &a2)
        : eq(a1, a2) {}

    bool eval(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
class in_set : public condition
{
    argument str;
    list_argument list;
public:
    in_set(std::auto_ptr<expression> & , std::auto_ptr<string_list> & );
    ~in_set();

    bool eval(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
class not_in_set : public condition
{
    argument str;
    list_argument list;
public:
    not_in_set(std::auto_ptr<expression> & , std::auto_ptr<string_list> & );
    ~not_in_set();

    bool eval(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
class match_regexp : public condition
{
    argument str;
    regexp_argument re;
public:
    match_regexp(std::auto_ptr<expression> & , std::auto_ptr<regexp> & );
    ~match_regexp();

    bool eval(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
class not_match_regexp : public condition
{
    match_regexp cond;
public:
    not_match_regexp(std::auto_ptr<expression> &s, std::auto_ptr<regexp> &r)
        : cond(s, r) {}

    bool eval(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
class cond_and : public condition
{
    not_null_ptr<const condition> cond1, cond2;
public:
    cond_and(std::auto_ptr<condition> & , std::auto_ptr<condition> & );
    ~cond_and();

    bool eval(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
class cond_or : public condition
{
    not_null_ptr<const condition> cond1, cond2;
public:
    cond_or(std::auto_ptr<condition> & , std::auto_ptr<condition> & );
    ~cond_or();

    bool eval(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
class cond_not : public condition
{
    not_null_ptr<const condition> cond;
public:
    explicit cond_not(std::auto_ptr<condition> & );
    ~cond_not();

    bool eval(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
