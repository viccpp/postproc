#ifndef __POSTPROC_DEFS_H
#define __POSTPROC_DEFS_H

#include<mfisoft/january/static_string.h>
#include<mfisoft/january/fastmap.h>
#include<mfisoft/january/regexp.h>
#include<map>
#include<string>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class map : public jan::fastmap<std::string,std::string>
{
    static const std::string empty_str;
public:
    // Extend standard map by const subscript operation:
    // if field was not found then return empty string
    using jan::fastmap<std::string,std::string>::operator[];
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
    jan::static_string msg;
protected:
    explicit simple_exception(const char *m) : msg(m) {}
public:
    const char *what() const throw();
};
//////////////////////////////////////////////////////////////////////////////
struct null_pointer : public exception
{
    const char *what() const throw();
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
    const char *what() const throw();
};
//////////////////////////////////////////////////////////////////////////////
struct invalid_function_arg : public simple_exception
{
    explicit invalid_function_arg(const char *msg)
        : simple_exception(msg) {}
};
//////////////////////////////////////////////////////////////////////////////

using mfisoft::january::regexp;

enum operation_result { nothing, break_script, discard_record };

#if __cplusplus >= 201103L
#define PP_NORETURN [[noreturn]]
#elif defined(__GNUC__) || defined(__clang__)
#define PP_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define PP_NORETURN __declspec(noreturn)
#else
#define PP_NORETURN
#endif

} // namespace

#endif // header guard
