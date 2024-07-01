#ifndef POSTPROC_HEX_H
#define POSTPROC_HEX_H

#include<string>
#include<iterator>
#include<mfisoft/bits.h>
#include<mfisoft/ascii.h>

namespace postproc {

//----------------------------------------------------------------------------
inline char lo_nibble_to_hex_upper(unsigned char b)
{
    return mfi::ascii::toxdigit_upper(mfi::lo_nibble(b));
}
//----------------------------------------------------------------------------
inline char hi_nibble_to_hex_upper(unsigned char b)
{
    return mfi::ascii::toxdigit_upper(mfi::hi_nibble(b));
}
//----------------------------------------------------------------------------
inline void append_base16_upper(unsigned char byte, std::string &s)
{
    s += hi_nibble_to_hex_upper(byte);
    s += lo_nibble_to_hex_upper(byte);
}
//----------------------------------------------------------------------------
template<class Cont>
void append_base16_upper(const Cont &c, std::string &s)
{
    for(unsigned char byte : c)
        append_base16_upper(byte, s);
}
//----------------------------------------------------------------------------
template<class Cont>
std::string to_base16_upper(const Cont &c)
{
    std::string res;
    res.reserve(std::distance(std::begin(c), std::end(c)) * 2);
    append_base16_upper(c, res);
    return res;
}
//----------------------------------------------------------------------------
inline int xdigit_to_number(char ch)
{
    return mfi::ascii::xdigit_to_number(ch);
}
//----------------------------------------------------------------------------

} // namespace

#endif // header
