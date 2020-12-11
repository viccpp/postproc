#include<postproc/functions.h>
#include<postproc/defs.h>
#include<mfisoft/january/bits.h>
#include<mfisoft/january/string.h>
#include<mfisoft/january/uuid.h>
#include<stdexcept>
#include<stdint.h>
#include<cstdlib> // for abs()
#ifdef HAVE_LLDIV
#include<stdlib.h> // for lldiv()
#endif
#include<cctype>
#include<algorithm>

namespace postproc { namespace functions {

//----------------------------------------------------------------------------
length::length(std::auto_ptr<expression> &e) : arg(e)
{
}
//----------------------------------------------------------------------------
length::~length()
{
}
//----------------------------------------------------------------------------
std::string length::value(const map &fields, const context &ctx) const
{
    return jan::string_buffer(16) << arg.value(fields, ctx).length();
}
//----------------------------------------------------------------------------
const char substr::func[] = "substr";
substr::substr(std::auto_ptr<expression> &str,
    std::auto_ptr<expression> &off, std::auto_ptr<expression> &cnt)
:
    str_expr(str), off_expr(off), count_expr(cnt, std::string::npos)
{
}
//----------------------------------------------------------------------------
substr::~substr()
{
}
//----------------------------------------------------------------------------
std::string substr::value(const map &fields, const context &ctx) const
{
    std::string st = str_expr.value(fields, ctx);
    off_t off = off_expr.value(fields, ctx);
    if(std::abs(off) > st.length()) return std::string(); // index is out of range

    if(off > 0) return st.substr(off - 1, count_expr.value(fields, ctx));
    // Assert: off < 0
    return st.substr(
        st.size() - static_cast<std::string::size_type>(-off),
        count_expr.value(fields, ctx)
    );
}
//----------------------------------------------------------------------------
replace_base_::replace_base_(std::auto_ptr<expression> &str,
            std::auto_ptr<regexp> &rexp, std::auto_ptr<expression> &repl)
    : str_expr(str), re(rexp), rep_text(repl)
{
}
//----------------------------------------------------------------------------
replace_base_::~replace_base_()
{
}
//----------------------------------------------------------------------------
replace::replace(std::auto_ptr<expression> &str,
        std::auto_ptr<regexp> &rexp, std::auto_ptr<expression> &repl)
    : replace_base_(str, rexp, repl)
{
}
//----------------------------------------------------------------------------
replace::~replace()
{
}
//----------------------------------------------------------------------------
std::string replace::value(const map &fields, const context &ctx) const
{
    return re.replace_first(
        str_expr.value(fields, ctx), rep_text.value(fields, ctx));
}
//----------------------------------------------------------------------------
replace_all::replace_all(std::auto_ptr<expression> &str,
        std::auto_ptr<regexp> &rexp, std::auto_ptr<expression> &repl)
    : replace_base_(str, rexp, repl)
{
}
//----------------------------------------------------------------------------
replace_all::~replace_all()
{
}
//----------------------------------------------------------------------------
std::string replace_all::value(const map &fields, const context &ctx) const
{
    return re.replace_all(
        str_expr.value(fields, ctx), rep_text.value(fields, ctx));
}
//----------------------------------------------------------------------------
upper::upper(std::auto_ptr<expression> &e) : arg(e)
{
}
//----------------------------------------------------------------------------
upper::~upper()
{
}
//----------------------------------------------------------------------------
std::string upper::value(const map &fields, const context &ctx) const
{
    std::string res = arg.value(fields, ctx);
    return jan::toupper(res);
}
//----------------------------------------------------------------------------
lower::lower(std::auto_ptr<expression> &e) : arg(e)
{
}
//----------------------------------------------------------------------------
lower::~lower()
{
}
//----------------------------------------------------------------------------
std::string lower::value(const map &fields, const context &ctx) const
{
    std::string res = arg.value(fields, ctx);
    return jan::tolower(res);
}
//----------------------------------------------------------------------------
reverse::reverse(std::auto_ptr<expression> &e) : arg(e)
{
}
//----------------------------------------------------------------------------
reverse::~reverse()
{
}
//----------------------------------------------------------------------------
std::string reverse::value(const map &fields, const context &ctx) const
{
    std::string res = arg.value(fields, ctx);
    std::reverse(res.begin(), res.end());
    return res;
}
//----------------------------------------------------------------------------
ltrim::ltrim(std::auto_ptr<expression> &a, const std::string &chrs)
    : arg(a), chars(chrs)
{
}
//----------------------------------------------------------------------------
ltrim::~ltrim()
{
}
//----------------------------------------------------------------------------
std::string ltrim::value(const map &fields, const context &ctx) const
{
    std::string res = arg.value(fields, ctx);
    jan::trim_left(res, chars.c_str());
    return res;
}
//----------------------------------------------------------------------------
ltrim_spaces::ltrim_spaces(std::auto_ptr<expression> &a) : arg(a)
{
}
//----------------------------------------------------------------------------
ltrim_spaces::~ltrim_spaces()
{
}
//----------------------------------------------------------------------------
std::string ltrim_spaces::value(const map &fields, const context &ctx) const
{
    std::string res = arg.value(fields, ctx);
    jan::trim_left(res);
    return res;
}
//----------------------------------------------------------------------------
rtrim::rtrim(std::auto_ptr<expression> &a, const std::string &chrs)
    : arg(a), chars(chrs)
{
}
//----------------------------------------------------------------------------
rtrim::~rtrim()
{
}
//----------------------------------------------------------------------------
std::string rtrim::value(const map &fields, const context &ctx) const
{
    std::string res = arg.value(fields, ctx);
    jan::trim_right(res, chars.c_str());
    return res;
}
//----------------------------------------------------------------------------
rtrim_spaces::rtrim_spaces(std::auto_ptr<expression> &a) : arg(a)
{
}
//----------------------------------------------------------------------------
rtrim_spaces::~rtrim_spaces()
{
}
//----------------------------------------------------------------------------
std::string rtrim_spaces::value(const map &fields, const context &ctx) const
{
    std::string res = arg.value(fields, ctx);
    jan::trim_right(res);
    return res;
}
//----------------------------------------------------------------------------
trim::trim(std::auto_ptr<expression> &a, const std::string &chrs)
    : arg(a), chars(chrs)
{
}
//----------------------------------------------------------------------------
trim::~trim()
{
}
//----------------------------------------------------------------------------
std::string trim::value(const map &fields, const context &ctx) const
{
    std::string res = arg.value(fields, ctx);
    jan::trim(res, chars.c_str());
    return res;
}
//----------------------------------------------------------------------------
trim_spaces::trim_spaces(std::auto_ptr<expression> &a) : arg(a)
{
}
//----------------------------------------------------------------------------
trim_spaces::~trim_spaces()
{
}
//----------------------------------------------------------------------------
std::string trim_spaces::value(const map &fields, const context &ctx) const
{
    std::string res = arg.value(fields, ctx);
    jan::trim(res);
    return res;
}
//----------------------------------------------------------------------------
const char lpad::func[] = "lpad";
lpad::lpad(std::auto_ptr<expression> &a1,
            std::auto_ptr<expression> &a2, char pad_chr)
    : str_expr(a1), len_expr(a2), pad_char(pad_chr)
{
}
//----------------------------------------------------------------------------
lpad::~lpad()
{
}
//----------------------------------------------------------------------------
std::string lpad::value(const map &fields, const context &ctx) const
{
    std::string str = str_expr.value(fields, ctx);
    size_t len = len_expr.value(fields, ctx);
    return str.length() >= len ? str : jan::pad_left(str, len, pad_char);
}
//----------------------------------------------------------------------------
hex2dec::hex2dec(std::auto_ptr<expression> &a) : arg(a)
{
}
//----------------------------------------------------------------------------
hex2dec::~hex2dec()
{
}
//----------------------------------------------------------------------------
std::string hex2dec::value(const map &fields, const context &ctx) const
{
    std::string st = arg.value(fields, ctx);
    if(st.empty()) return st;

    uintmax_t res = 0;
    static const uintmax_t threshold_mask = ~(~uintmax_t(0) >> 4);
    for(std::string::const_iterator
            it = st.begin(); it != st.end(); ++it)
    {
        if(res & threshold_mask) return st; // the number is too big
        int dig = jan::hex_to_number(*it);
        if(dig == -1) return st; // not a hex digit
        res = (res << 4) | dig;
    }
    return jan::string_buffer(10) << res;
}
//----------------------------------------------------------------------------
dec2hex::dec2hex(std::auto_ptr<expression> &a) : arg(a)
{
}
//----------------------------------------------------------------------------
dec2hex::~dec2hex()
{
}
//----------------------------------------------------------------------------
std::string dec2hex::value(const map &fields, const context &ctx) const
{
    std::string st = arg.value(fields, ctx);
    if(st.empty()) return st;

    uintmax_t dec;
    try {
        jan::decimal_to_number(st, dec);
    } catch(const std::exception & ) {
        return st;
    }
    char hex[2 * sizeof dec];
    char * const hex_end = hex + sizeof hex;
    char *p = hex_end;
    do {
        uint8_t b = dec & 0xFFU;
        *--p = jan::lo_nibble_to_hex(b);
        *--p = jan::hi_nibble_to_hex(b);
        dec >>= 8;
    } while(dec != 0);
    // Remove leading '0' if length is more than 1
    if(*p == '0' && p != hex_end - 1) p++;
    return std::string(p, hex_end);
}
//----------------------------------------------------------------------------
map_func::map_func(std::auto_ptr<expression> &val,
                    mapping::list &mlist, std::auto_ptr<expression> &def)
    : value_expr(val), m(mlist), def_value(def)
{
}
//----------------------------------------------------------------------------
map_func::~map_func()
{
}
//----------------------------------------------------------------------------
std::string map_func::value(const map &fields, const context &ctx) const
{
    std::string val = value_expr.value(fields, ctx);
    if(const expression *p = m.find(val)) return p->value(fields, ctx);
    return def_value.get() ? def_value->value(fields, ctx) : val;
}
//----------------------------------------------------------------------------
const char iadd::func[] = "iadd";
iadd::iadd(std::auto_ptr<expression> &a1, std::auto_ptr<expression> &a2)
    : arg1(a1), arg2(a2)
{
}
//----------------------------------------------------------------------------
iadd::~iadd()
{
}
//----------------------------------------------------------------------------
std::string iadd::value(const map &fields, const context &ctx) const
{
    return jan::string_buffer(16) <<
        (arg1.value(fields, ctx) + arg2.value(fields, ctx));
}
//----------------------------------------------------------------------------
const char isub::func[] = "isub";
isub::isub(std::auto_ptr<expression> &a1, std::auto_ptr<expression> &a2)
    : arg1(a1), arg2(a2)
{
}
//----------------------------------------------------------------------------
isub::~isub()
{
}
//----------------------------------------------------------------------------
std::string isub::value(const map &fields, const context &ctx) const
{
    return jan::string_buffer(16) <<
        (arg1.value(fields, ctx) - arg2.value(fields, ctx));
}
//----------------------------------------------------------------------------
const char idiv::func[] = "idiv";
idiv::idiv(std::auto_ptr<expression> &a1, std::auto_ptr<expression> &a2)
    : arg1(a1), arg2(a2)
{
}
//----------------------------------------------------------------------------
idiv::~idiv()
{
}
//----------------------------------------------------------------------------
std::string idiv::value(const map &fields, const context &ctx) const
{
    return jan::string_buffer(16) <<
        (arg1.value(fields, ctx) / arg2.value(fields, ctx));
}
//----------------------------------------------------------------------------
const char idiv_ceil::func[] = "idiv_ceil";
idiv_ceil::idiv_ceil(std::auto_ptr<expression> &a1, std::auto_ptr<expression> &a2)
    : arg1(a1), arg2(a2)
{
}
//----------------------------------------------------------------------------
idiv_ceil::~idiv_ceil()
{
}
//----------------------------------------------------------------------------
std::string idiv_ceil::value(const map &fields, const context &ctx) const
{
#ifdef HAVE_LLDIV
    ::lldiv_t d = ::lldiv
#else
    std::ldiv_t d = std::ldiv
#endif
        (arg1.value(fields, ctx), arg2.value(fields, ctx));
    return jan::string_buffer(16) << (d.rem == 0 ? d.quot : d.quot + 1);
}
//----------------------------------------------------------------------------
const char imul::func[] = "imul";
imul::imul(std::auto_ptr<expression> &a1, std::auto_ptr<expression> &a2)
    : arg1(a1), arg2(a2)
{
}
//----------------------------------------------------------------------------
imul::~imul()
{
}
//----------------------------------------------------------------------------
std::string imul::value(const map &fields, const context &ctx) const
{
    return jan::string_buffer(16) <<
        (arg1.value(fields, ctx) * arg2.value(fields, ctx));
}
//----------------------------------------------------------------------------
sift::sift(std::auto_ptr<expression> &a, const std::string &chrs)
    : arg(a), chars(chrs)
{
}
//----------------------------------------------------------------------------
sift::~sift()
{
}
//----------------------------------------------------------------------------
std::string sift::value(const map &fields, const context &ctx) const
{
    std::string res = arg.value(fields, ctx);
    jan::sift(res, chars.c_str());
    return res;
}
//----------------------------------------------------------------------------
sift_nonprint::sift_nonprint(std::auto_ptr<expression> &a) : arg(a)
{
}
//----------------------------------------------------------------------------
sift_nonprint::~sift_nonprint()
{
}
//----------------------------------------------------------------------------
struct sift_nonprint::is_non_printable
{
    bool operator()(char ch) const { return !std::isprint(ch); }
};
//----------------------------------------------------------------------------
std::string sift_nonprint::value(const map &fields, const context &ctx) const
{
    std::string res = arg.value(fields, ctx);
    jan::sift_if(res, is_non_printable());
    return res;
}
//----------------------------------------------------------------------------
const char bit_and::func[] = "bit_and";
bit_and::bit_and(std::auto_ptr<expression> &a1, std::auto_ptr<expression> &a2)
    : arg1(a1), arg2(a2)
{
}
//----------------------------------------------------------------------------
bit_and::~bit_and()
{
}
//----------------------------------------------------------------------------
std::string bit_and::value(const map &fields, const context &ctx) const
{
    return jan::string_buffer(16) <<
        (arg1.value(fields, ctx) & arg2.value(fields, ctx));
}
//----------------------------------------------------------------------------
const char bit_or::func[] = "bit_or";
bit_or::bit_or(std::auto_ptr<expression> &a1, std::auto_ptr<expression> &a2)
    : arg1(a1), arg2(a2)
{
}
//----------------------------------------------------------------------------
bit_or::~bit_or()
{
}
//----------------------------------------------------------------------------
std::string bit_or::value(const map &fields, const context &ctx) const
{
    return jan::string_buffer(16) <<
        (arg1.value(fields, ctx) | arg2.value(fields, ctx));
}
//----------------------------------------------------------------------------
const char bit_shl::func[] = "bit_shl";
bit_shl::bit_shl(std::auto_ptr<expression> &a1, std::auto_ptr<expression> &a2)
    : num(a1), pos(a2)
{
}
//----------------------------------------------------------------------------
bit_shl::~bit_shl()
{
}
//----------------------------------------------------------------------------
std::string bit_shl::value(const map &fields, const context &ctx) const
{
    return jan::string_buffer(16) <<
        (num.value(fields, ctx) << pos.value(fields, ctx));
}
//----------------------------------------------------------------------------
const char bit_shr::func[] = "bit_shr";
bit_shr::bit_shr(std::auto_ptr<expression> &a1, std::auto_ptr<expression> &a2)
    : num(a1), pos(a2)
{
}
//----------------------------------------------------------------------------
bit_shr::~bit_shr()
{
}
//----------------------------------------------------------------------------
std::string bit_shr::value(const map &fields, const context &ctx) const
{
    return jan::string_buffer(16) <<
        (num.value(fields, ctx) >> pos.value(fields, ctx));
}
//----------------------------------------------------------------------------
const char bit_not::func[] = "bit_not";
bit_not::bit_not(std::auto_ptr<expression> &arg)
    : num(arg)
{
}
//----------------------------------------------------------------------------
bit_not::~bit_not()
{
}
//----------------------------------------------------------------------------
std::string bit_not::value(const map &fields, const context &ctx) const
{
    return jan::string_buffer(16) << ~num.value(fields, ctx);
}
//----------------------------------------------------------------------------
ip6_to_ip4::ip6_to_ip4(std::auto_ptr<expression> &arg)
:
    expr(arg),
    re("^(?:::|(?:0{1,4}:){5})FFFF:"
       "([[:xdigit:]]{0,2}?)" "([[:xdigit:]]{1,2}):"
       "([[:xdigit:]]{0,2}?)" "([[:xdigit:]]{1,2})$", "i")
{
}
//----------------------------------------------------------------------------
ip6_to_ip4::~ip6_to_ip4()
{
}
//----------------------------------------------------------------------------
inline unsigned ip6_to_ip4::to_dec(jan::string_ref s)
{
    jan::string_ref::const_iterator it = s.begin();
    if(it == s.end()) return 0;
    int res = jan::hex_to_number(*it);
    if(++it != s.end()) { res <<= 4; res |= jan::hex_to_number(*it); }
    return res;
}
//----------------------------------------------------------------------------
std::string ip6_to_ip4::value(const map &fields, const context &ctx) const
{
    std::string ipv6 = expr.value(fields, ctx);
    regexp::match_results r;
    if(!re.exec(ipv6, r)) return ipv6;
    jan::string_buffer ipv4(16);
    ipv4 << to_dec(r[1]) << '.' << to_dec(r[2]) << '.'
         << to_dec(r[3]) << '.' << to_dec(r[4]);
    return ipv4;
}
//----------------------------------------------------------------------------
ip4_to_hex::ip4_to_hex(std::auto_ptr<expression> &arg) : expr(arg)
{
}
//----------------------------------------------------------------------------
ip4_to_hex::~ip4_to_hex()
{
}
//----------------------------------------------------------------------------
bool ip4_to_hex::to_hex(std::string::const_iterator it1,
    std::string::const_iterator it2, std::string &res)
{
    uint8_t b;
    try {
        jan::decimal_to_number_range(it1, it2, b);
    } catch(const std::exception & ) {
        return false;
    }
    res += jan::hi_nibble_to_hex(b);
    res += jan::lo_nibble_to_hex(b);
    return true;
}
//----------------------------------------------------------------------------
std::string ip4_to_hex::value(const map &fields, const context &ctx) const
{
    std::string ip = expr.value(fields, ctx);
    jan::string_buffer res(8);
    std::string::size_type pos1 = 0;
    for(int c = 3; c--;)
    {
        std::string::size_type pos2 = ip.find('.', pos1);
        if(pos2 == std::string::npos ||
            !to_hex(ip.begin() + pos1, ip.begin() + pos2, res)) return ip;
        pos1 = pos2 + 1;
    }
    if(!to_hex(ip.begin() + pos1, ip.end(), res)) return ip;
    return res;
}
//----------------------------------------------------------------------------
std::string uuid::value(const map & , const context & ) const
{
    unsigned char uuid[16];
    jan::generate_random_uuid(uuid);
    std::string res;
    jan::uuid_to_hex(uuid, res);
    return res;
}
//----------------------------------------------------------------------------

}} // namespace
