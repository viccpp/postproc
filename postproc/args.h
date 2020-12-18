#ifndef POSTPROC_ARGS_H
#define POSTPROC_ARGS_H

#include<postproc/defs.h>
#include<postproc/smart_ptr.h>
#include<postproc/exprs.h>
#include<postproc/string_list.h>
#include<mfisoft/str2num.h>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
// Numbers parser (string-to-number converter)
template<class Num>
struct num_parser
{
    typedef Num value_type;
    Num operator()(const std::string &val, const char *func, int i) const
        { return parse(val, func, i); }
    static Num parse(const std::string & , const char * , int );
};
//////////////////////////////////////////////////////////////////////////////
template<class Num>
struct num_or_empty_parser : private num_parser<Num>
{
    Num operator()(const std::string &val, const char *func, int i) const
    {
        if(val.empty()) throw empty_function_arg();
        return this->parse(val, func, i);
    }
};
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
template<class Num>
struct numeric_argument_expr
{
    typedef Num value_type;
    virtual ~numeric_argument_expr() = default;
    virtual Num value(const map & , const context & ,
                                const char * , int ) const = 0;
};
//////////////////////////////////////////////////////////////////////////////
template<class Num>
class numeric_argument_const : public numeric_argument_expr<Num>
{
    Num n;
public:
    template<class Parser>
    numeric_argument_const(
            const string_literal &str, const char *func, int i, Parser p)
        : n(p(str, func, i)) {}
    explicit numeric_argument_const(Num val) : n(val) {}
    Num value(const map & , const context & , const char * , int ) const override
    {
        return n;
    }
};
//////////////////////////////////////////////////////////////////////////////
template<class Num, class Parser = num_parser<Num> >
class numeric_argument_var :
    public numeric_argument_expr<Num>, private Parser
{
    not_null_ptr<const expression> expr;
    const Parser &parser() const { return *this; }
public:
    typedef Parser parser_type;
    explicit numeric_argument_var(
            std::unique_ptr<expression> &arg, Parser p = Parser())
        : Parser(p), expr(arg) {}
    Num value(const map &fields, const context &ctx,
                                    const char *func, int i) const override
    {
        return parser()(expr->value(fields, ctx), func, i);
    }
};
//////////////////////////////////////////////////////////////////////////////
// Base part of numeric_argument
template<class Num>
class numeric_argument_
{
    not_null_ptr<const numeric_argument_expr<Num> > arg;
    template<class Parser>
    static numeric_argument_expr<Num> *create(
        std::unique_ptr<expression> & , const char * , int , Parser );
public:
    typedef Num value_type;

    template<class Parser>
    numeric_argument_(
        std::unique_ptr<expression> & , const char * , int , Parser );
    template<class Parser>
    numeric_argument_(
        std::unique_ptr<expression> & , Num , const char * , int , Parser );

    Num value(const map &fields, const context &ctx,
                                    const char *func, int i) const
    {
        return arg->value(fields, ctx, func, i);
    }
};
//////////////////////////////////////////////////////////////////////////////
[[noreturn]] void throw_invalid_numeric_arg(const std::string & , const char * , int );
[[noreturn]] void throw_invalid_numeric_arg(const char * , const char * );
//----------------------------------------------------------------------------
template<class Num>
Num num_parser<Num>::parse(const std::string &val, const char *func, int i)
{
    mfi::decimal_parser<Num> p;
    if(p.parse(val) != mfi::number_parse_status::ok)
        throw_invalid_numeric_arg(val, func, i);
    return p.result();
}
//----------------------------------------------------------------------------
template<class Num>
template<class Parser>
inline numeric_argument_expr<Num> *numeric_argument_<Num>::create(
    std::unique_ptr<expression> &expr, const char *func, int i, Parser p)
{
    if(auto lit = dynamic_cast<string_literal *>(expr.get()))
        return new numeric_argument_const<Num>(*lit, func, i, p);
    return new numeric_argument_var<Num, Parser>(expr, p);
}
//----------------------------------------------------------------------------
template<class Num>
template<class Parser>
numeric_argument_<Num>::numeric_argument_(
    std::unique_ptr<expression> &expr, const char *func, int i, Parser p)
:
    arg(not_null, create(expr, func, i, p))
{
}
//----------------------------------------------------------------------------
template<class Num>
template<class Parser>
numeric_argument_<Num>::numeric_argument_(
    std::unique_ptr<expression> &expr, Num def, const char *func, int i, Parser p)
:
    arg(not_null, expr.get() ?
        create(expr, func, i, p) : new numeric_argument_const<Num>(def))
{
}
//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// Interface classes
//////////////////////////////////////////////////////////////////////////////
class argument
{
    not_null_ptr<const expression> expr;
public:
    explicit argument(std::unique_ptr<expression> &arg) : expr(arg) {}
    std::string value(const map &fields, const context &ctx) const
        { return expr->value(fields, ctx); }
};
//////////////////////////////////////////////////////////////////////////////
template<class Num, const char *Func, unsigned I, class Parser = num_parser<Num> >
class numeric_argument : public numeric_argument_<Num>
{
    typedef numeric_argument_<Num> base;
public:
    typedef Parser parser_type;
    explicit numeric_argument(std::unique_ptr<expression> &expr)
        : base(expr, Func, I, Parser()) {}
    numeric_argument(std::unique_ptr<expression> &expr, Num def)
        : base(expr, def, Func, I, Parser()) {}
    Num value(const map &fields, const context &ctx) const
        { return base::value(fields, ctx, Func, I); }
};
//////////////////////////////////////////////////////////////////////////////
class list_argument
{
    not_null_ptr<const string_list> list;
public:
    explicit list_argument(std::unique_ptr<string_list> &arg) : list(arg) {}
    const string_list::values_t &get_values(const context &ctx) const
        { return list->get_values(ctx); }
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
