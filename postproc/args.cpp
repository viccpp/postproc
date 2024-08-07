#include<postproc/args.h>
#include<__vic/string_buffer.h>

namespace postproc {

//----------------------------------------------------------------------------
void throw_invalid_numeric_arg(const std::string &val, const char *func, int i)
{
    throw invalid_function_arg(__vic::msg(64) <<
        func << "(): not a valid number given to arg" << i <<
        ": \"" << val << '"');
}
//----------------------------------------------------------------------------
void throw_invalid_numeric_arg(const char *func, const char *msg)
{
    throw invalid_function_arg(__vic::msg(64) << func << "(): " << msg);
}
//----------------------------------------------------------------------------

} // namespace
