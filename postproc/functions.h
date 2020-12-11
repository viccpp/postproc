#ifndef __POSTPROC_FUNCTIONS_H
#define __POSTPROC_FUNCTIONS_H

#include<postproc/defs.h>
#include<postproc/args.h>
#include<postproc/mapping.h>
#include<mfisoft/january/memory.h>
#include<mfisoft/january/string_ref.h>
#include<list>

namespace postproc { namespace functions {

//////////////////////////////////////////////////////////////////////////////
// length(<string>)
class length : public function
{
    argument arg;
public:
    explicit length(std::auto_ptr<expression> & );
    ~length();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// substr(<string>, <offset> [, <count>])
// Requires: off != 0 && count >= 0
class substr : public function
{
    typedef int off_t;
    static const char func[];
    struct off_parser : private num_parser<off_t>
    {
        value_type operator()(
            const std::string &val, const char *func, int i) const
        {
            value_type n = this->parse(val, func, i);
            if(n == 0) throw_invalid_numeric_arg(func, "offset can't be 0");
            return n;
        }
    };
    argument str_expr;
    numeric_argument<off_t, func, 2, off_parser> off_expr;
    numeric_argument<std::string::size_type, func, 3> count_expr;
public:
    substr(std::auto_ptr<expression> & ,
        std::auto_ptr<expression> & , std::auto_ptr<expression> & );
    substr(std::auto_ptr<expression> & , std::auto_ptr<expression> & );
    ~substr();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// Internal base class
class replace_base_
{
protected:
    argument str_expr;
    regexp_argument re;
    argument rep_text;

    replace_base_(std::auto_ptr<expression> & ,
        std::auto_ptr<regexp> & , std::auto_ptr<expression> & );
    ~replace_base_();
};
//////////////////////////////////////////////////////////////////////////////
// replace(<string>, <regexp>, <replace-text>)
class replace : public function, private replace_base_
{
public:
    replace(std::auto_ptr<expression> & ,
        std::auto_ptr<regexp> & , std::auto_ptr<expression> & );
    ~replace();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// replace_all(<string>, <regexp>, <replace-text>)
class replace_all : public function, private replace_base_
{
public:
    replace_all(std::auto_ptr<expression> & ,
        std::auto_ptr<regexp> & , std::auto_ptr<expression> & );
    ~replace_all();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// upper(<string>)
class upper : public function
{
    argument arg;
public:
    explicit upper(std::auto_ptr<expression> & );
    ~upper();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// lower(<string>)
class lower : public function
{
    argument arg;
public:
    explicit lower(std::auto_ptr<expression> & );
    ~lower();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// reverse(<string>)
class reverse : public function
{
    argument arg;
public:
    explicit reverse(std::auto_ptr<expression> & );
    ~reverse();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// ltrim(<string>, <chars>)
class ltrim : public function
{
    argument arg;
    std::string chars;
public:
    ltrim(std::auto_ptr<expression> & , const std::string & );
    ~ltrim();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// ltrim(<string>)
class ltrim_spaces : public function
{
    argument arg;
public:
    explicit ltrim_spaces(std::auto_ptr<expression> & );
    ~ltrim_spaces();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// rtrim(<string>, <chars>)
class rtrim : public function
{
    argument arg;
    std::string chars;
public:
    rtrim(std::auto_ptr<expression> & , const std::string & );
    ~rtrim();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// rtrim(<string>)
class rtrim_spaces : public function
{
    argument arg;
public:
    explicit rtrim_spaces(std::auto_ptr<expression> & );
    ~rtrim_spaces();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// trim(<string>, <chars>)
class trim : public function
{
    argument arg;
    std::string chars;
public:
    trim(std::auto_ptr<expression> & , const std::string & );
    ~trim();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// trim(<string>)
class trim_spaces : public function
{
    argument arg;
public:
    explicit trim_spaces(std::auto_ptr<expression> & );
    ~trim_spaces();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// lpad(<string>, <len>, <pad_char>)
class lpad : public function
{
    static const char func[];
    argument str_expr;
    numeric_argument<size_t, func, 2> len_expr;
    char pad_char;
public:
    lpad(std::auto_ptr<expression> & , std::auto_ptr<expression> & , char );
    ~lpad();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// hex2dec(<hex-string>)
class hex2dec : public function
{
    argument arg;
public:
    explicit hex2dec(std::auto_ptr<expression> & );
    ~hex2dec();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// dec2hex(<dec-string>)
class dec2hex : public function
{
    argument arg;
public:
    explicit dec2hex(std::auto_ptr<expression> & );
    ~dec2hex();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// map(<value>, (<v1>, <v2>, ...) -> <new-value1>, ... [, <default-value>])
class map_func : public function
{
    argument value_expr;
    mapping m;
    std::auto_ptr<expression> def_value; // can be null
public:
    map_func(std::auto_ptr<expression> & ,
                        mapping::list & , std::auto_ptr<expression> & );
    ~map_func();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// iadd(<number1>, <number2>)
class iadd : public function
{
    static const char func[];
    numeric_argument<long long, func, 1> arg1;
    numeric_argument<long long, func, 2> arg2;
public:
    iadd(std::auto_ptr<expression> & , std::auto_ptr<expression> & );
    ~iadd();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// isub(<number1>, <number2>)
class isub : public function
{
    static const char func[];
    numeric_argument<long long, func, 1> arg1;
    numeric_argument<long long, func, 2> arg2;
public:
    isub(std::auto_ptr<expression> & , std::auto_ptr<expression> & );
    ~isub();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// For idiv() and others
template<class Num>
struct divisor_parser : private num_parser<Num>
{
    Num operator()(const std::string &val, const char *func, int i) const
    {
        Num n = static_cast<const num_parser<Num> &>(*this)(val, func, i);
        if(n == 0) throw_invalid_numeric_arg(func, "divisor can't be 0");
        return n;
    }
};
//////////////////////////////////////////////////////////////////////////////
// idiv(<number1>, <number2>)
class idiv : public function
{
    static const char func[];
    numeric_argument<long long, func, 1> arg1;
    numeric_argument<long long, func, 2, divisor_parser<long long> > arg2;
public:
    idiv(std::auto_ptr<expression> & , std::auto_ptr<expression> & );
    ~idiv();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// idiv_ceil(<number1>, <number2>)
class idiv_ceil : public function
{
    static const char func[];
    typedef
#ifdef HAVE_LLDIV
        long
#endif
        long int_t;
    numeric_argument<int_t, func, 1> arg1;
    numeric_argument<int_t, func, 2, divisor_parser<int_t> > arg2;
public:
    idiv_ceil(std::auto_ptr<expression> & , std::auto_ptr<expression> & );
    ~idiv_ceil();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// imul(<number1>, <number2>)
class imul : public function
{
    static const char func[];
    numeric_argument<long long, func, 1> arg1;
    numeric_argument<long long, func, 2> arg2;
public:
    imul(std::auto_ptr<expression> & , std::auto_ptr<expression> & );
    ~imul();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// sift(<string>, <chars>)
class sift : public function
{
    argument arg;
    std::string chars;
public:
    sift(std::auto_ptr<expression> & , const std::string & );
    ~sift();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// sift_nonprint(<string>)
class sift_nonprint : public function
{
    argument arg;
    struct is_non_printable;
public:
    explicit sift_nonprint(std::auto_ptr<expression> & );
    ~sift_nonprint();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// bit_and(<number1>, <number2>)
class bit_and : public function
{
    static const char func[];
    numeric_argument<unsigned long long, func, 1> arg1;
    numeric_argument<unsigned long long, func, 2> arg2;
public:
    bit_and(std::auto_ptr<expression> & , std::auto_ptr<expression> & );
    ~bit_and();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// bit_or(<number1>, <number2>)
class bit_or : public function
{
    static const char func[];
    numeric_argument<unsigned long long, func, 1> arg1;
    numeric_argument<unsigned long long, func, 2> arg2;
public:
    bit_or(std::auto_ptr<expression> & , std::auto_ptr<expression> & );
    ~bit_or();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// bit_shl(<number>, <positions>)
class bit_shl : public function
{
    static const char func[];
    numeric_argument<unsigned long long, func, 1> num;
    numeric_argument<unsigned, func, 2> pos;
public:
    bit_shl(std::auto_ptr<expression> & , std::auto_ptr<expression> & );
    ~bit_shl();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// bit_shr(<number>, <positions>)
class bit_shr : public function
{
    static const char func[];
    numeric_argument<unsigned long long, func, 1> num;
    numeric_argument<unsigned, func, 2> pos;
public:
    bit_shr(std::auto_ptr<expression> & , std::auto_ptr<expression> & );
    ~bit_shr();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// bit_not(<number>)
class bit_not : public function
{
    static const char func[];
    numeric_argument<unsigned long long, func, 1> num;
public:
    explicit bit_not(std::auto_ptr<expression> & );
    ~bit_not();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// ip6_to_ip4(<ipv6-address>)
class ip6_to_ip4 : public function
{
    argument expr;
    regexp re;
    static unsigned to_dec(jan::string_ref );
public:
    explicit ip6_to_ip4(std::auto_ptr<expression> & );
    ~ip6_to_ip4();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// ip4_to_hex(<ipv4-address>)
class ip4_to_hex : public function
{
    argument expr;
    static bool to_hex(std::string::const_iterator ,
                        std::string::const_iterator , std::string & );
public:
    explicit ip4_to_hex(std::auto_ptr<expression> & );
    ~ip4_to_hex();

    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////
// uuid()
class uuid : public function
{
public:
    std::string value(const map & , const context & ) const; // override
};
//////////////////////////////////////////////////////////////////////////////

}} // namespace

#endif // header guard
