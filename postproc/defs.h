#ifndef POSTPROC_DEFS_H
#define POSTPROC_DEFS_H

#include<__vic/readonly_cstring.h>
#include<initializer_list>
#include<unordered_map>
#include<string>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class map : public std::unordered_map<std::string,std::string>
{
    typedef std::unordered_map<std::string, std::string> base;
    static const std::string empty_str;
public:
    map() = default;
    map(std::initializer_list<value_type> il) : base(il) {}

    // Extend standard map by const subscript operation:
    // if field was not found then return empty string
    using base::operator[];
    const std::string &operator[](const std::string &key) const
    {
        const_iterator it = find(key);
        return it != end() ? it->second : empty_str;
    }
};
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Base class for all exceptions
class exception : public std::exception
{
};
//////////////////////////////////////////////////////////////////////////////
// Implementation base class
class simple_exception : public exception
{
    __vic::readonly_cstring msg;
protected:
    explicit simple_exception(const char *m) : msg(m) {}
public:
    const char *what() const noexcept;
};
//////////////////////////////////////////////////////////////////////////////
struct null_pointer : public exception
{
    const char *what() const noexcept;
};
//////////////////////////////////////////////////////////////////////////////
struct undefined_constant : public simple_exception
{
    explicit undefined_constant(const char *msg)
        : simple_exception(msg) {}
};
//////////////////////////////////////////////////////////////////////////////
struct unknown_session_constant : public undefined_constant
{
    explicit unknown_session_constant(const std::string & );
};
//////////////////////////////////////////////////////////////////////////////
struct unknown_named_list : public undefined_constant
{
    explicit unknown_named_list(const std::string & );
};
//////////////////////////////////////////////////////////////////////////////
struct empty_function_arg : public exception
{
    const char *what() const noexcept;
};
//////////////////////////////////////////////////////////////////////////////
struct invalid_function_arg : public simple_exception
{
    explicit invalid_function_arg(const char *msg)
        : simple_exception(msg) {}
};
//////////////////////////////////////////////////////////////////////////////

enum class operation_result { nothing, break_script, discard_record };

} // namespace

#endif // header guard
