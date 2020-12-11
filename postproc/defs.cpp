#include<postproc/defs.h>
#include<mfisoft/january/string_buffer.h>

namespace postproc {

const std::string map::empty_str;

//----------------------------------------------------------------------------
const char *null_pointer::what() const throw()
{
    return "Null pointer argument specified";
}
//----------------------------------------------------------------------------
const char *empty_function_arg::what() const throw()
{
    return "Function argument has empty value";
}
//----------------------------------------------------------------------------
const char *simple_exception::what() const throw()
{
    return msg;
}
//----------------------------------------------------------------------------
unknown_session_constant::unknown_session_constant(const std::string &name)
    : undefined_constant(jan::msg(128) << "Unknown session constant: $" << name)
{
}
//----------------------------------------------------------------------------
unknown_named_list::unknown_named_list(const std::string &name)
    : undefined_constant(jan::msg(128) << "Unknown named list: @" << name)
{
}
//----------------------------------------------------------------------------

} // namespace
