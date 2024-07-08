#include<postproc/defs.h>
#include<__vic/string_buffer.h>

namespace postproc {

const std::string map::empty_str{};

//----------------------------------------------------------------------------
const char *null_pointer::what() const noexcept
{
    return "Null pointer argument specified";
}
//----------------------------------------------------------------------------
const char *empty_function_arg::what() const noexcept
{
    return "Function argument has empty value";
}
//----------------------------------------------------------------------------
const char *simple_exception::what() const noexcept
{
    return msg;
}
//----------------------------------------------------------------------------
unknown_session_constant::unknown_session_constant(const std::string &name)
    : undefined_constant(__vic::msg(64) << "Unknown session constant: $" << name)
{
}
//----------------------------------------------------------------------------
unknown_named_list::unknown_named_list(const std::string &name)
    : undefined_constant(__vic::msg(64) << "Unknown named list: @" << name)
{
}
//----------------------------------------------------------------------------

} // namespace
