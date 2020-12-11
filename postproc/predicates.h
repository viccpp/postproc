#ifndef __POSTPROC_PREDICATES_H
#define __POSTPROC_PREDICATES_H

#include<postproc/defs.h>
#include<postproc/condition.h>
#include<postproc/args.h>

namespace postproc { namespace predicates {

//////////////////////////////////////////////////////////////////////////////
// ?empty(<str>)
class empty : public predicate
{
    argument arg;
public:
    explicit empty(std::auto_ptr<expression> & );
    ~empty();

    bool eval(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// ?starts_with(<str>, <prefix>)
class starts_with : public predicate
{
    argument arg1, arg2;
public:
    starts_with(std::auto_ptr<expression> & , std::auto_ptr<expression> & );
    ~starts_with();

    bool eval(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// ?lt(<number1>, <number2>)
class lt : public predicate
{
    static const char func[];
    numeric_argument<long long, func, 1> arg1;
    numeric_argument<long long, func, 2> arg2;
public:
    lt(std::auto_ptr<expression> & , std::auto_ptr<expression> & );
    ~lt();

    bool eval(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// ?gt(<number1>, <number2>)
class gt : public predicate
{
    static const char func[];
    numeric_argument<long long, func, 1> arg1;
    numeric_argument<long long, func, 2> arg2;
public:
    gt(std::auto_ptr<expression> & , std::auto_ptr<expression> & );
    ~gt();

    bool eval(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// ?le(<number1>, <number2>)
class le : public predicate
{
    static const char func[];
    numeric_argument<long long, func, 1> arg1;
    numeric_argument<long long, func, 2> arg2;
public:
    le(std::auto_ptr<expression> & , std::auto_ptr<expression> & );
    ~le();

    bool eval(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// ?ge(<number1>, <number2>)
class ge : public predicate
{
    static const char func[];
    numeric_argument<long long, func, 1> arg1;
    numeric_argument<long long, func, 2> arg2;
public:
    ge(std::auto_ptr<expression> & , std::auto_ptr<expression> & );
    ~ge();

    bool eval(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////

}} // namespace

#endif // header guard
