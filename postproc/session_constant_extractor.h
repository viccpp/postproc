#ifndef __POSTPROC_SESSION_CONSTANT_EXTRACTOR_H
#define __POSTPROC_SESSION_CONSTANT_EXTRACTOR_H

#include<postproc/defs.h>
#include<string>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class session_constant_extractor
{
public:
    virtual ~session_constant_extractor() {}

    virtual std::string get(const std::string & ) const = 0;

    PP_NORETURN
    static void throw_unknown_session_constant(const std::string &name)
        { throw unknown_session_constant(name); }
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
