#ifndef POSTPROC_OPERATIONS_H
#define POSTPROC_OPERATIONS_H

#include<postproc/operation.h>
#include<postproc/args.h>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class assignment : public operation
{
    not_null_ptr<field> fld;
    argument expr;
public:
    assignment(unique_ptr<field> , unique_ptr<expression> );
    ~assignment();

    result_type eval(const map & , map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class swap_op : public operation
{
    not_null_ptr<field> f1, f2;
public:
    swap_op(unique_ptr<field> , unique_ptr<field> );
    ~swap_op();

    result_type eval(const map & , map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class discard : public operation
{
public:
    result_type eval(const map & , map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////
class break_ : public operation
{
public:
    result_type eval(const map & , map & , const context & ) const override;
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
