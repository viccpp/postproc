#ifndef POSTPROC_EXPRS_H
#define POSTPROC_EXPRS_H

#include<postproc/expr.h>
#include<postproc/smart_ptr.h>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class field : public primary_expr
{
    std::string name;
public:
    explicit field(const std::string & );
    ~field();

    void set_value(const std::string &v, map &fields) const
        { fields[name] = v; }
    std::string value(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class string_literal : public primary_expr, public std::string
{
public:
    explicit string_literal(const std::string &s) : std::string(s) {}
    ~string_literal();

    std::string value(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class integer_literal : public string_literal
{
public:
    explicit integer_literal(const std::string &s) : string_literal(s) {}
};
//////////////////////////////////////////////////////////////////////////////
class session_constant : public primary_expr
{
    std::string name;
public:
    explicit session_constant(const std::string & );
    ~session_constant();

    std::string value(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class concatenation : public expression
{
    not_null_ptr<const expression> arg1, arg2;
public:
    concatenation(std::unique_ptr<expression> & , std::unique_ptr<expression> & );
    ~concatenation();

    std::string value(const map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
