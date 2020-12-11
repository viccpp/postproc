#ifndef __POSTPROC_OPERATION_H
#define __POSTPROC_OPERATION_H

#include<postproc/defs.h>

namespace postproc {

class context; // forward

//////////////////////////////////////////////////////////////////////////////
class operation
{
public:
    typedef operation_result result_type;

    virtual ~operation() {}

    virtual result_type eval(const map & , map & , const context & ) const = 0;
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
