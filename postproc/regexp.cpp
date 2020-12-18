#include<postproc/regexp.h>
#include<mfisoft/string_buffer.h>

namespace postproc {

//----------------------------------------------------------------------------
regexp::regexp(const std::string &pattern, const std::string &opts)
{
    int options = 0;
    for(auto opt : opts)
        switch(opt)
        {
            case 'i': options |= PCRE_CASELESS; break;
            case 'x': options |= PCRE_EXTENDED; break;
            case 'm': options |= PCRE_MULTILINE; break;
            case 's': options |= PCRE_DOTALL; break;
            default: throw error(mfi::msg(64) <<
                "RegExp compilation failed: unknown option '" << opt << '\'');
        }
    const char *errmsg;
    int erroffset;
    re = ::pcre_compile(pattern.c_str(), options, &errmsg, &erroffset, nullptr);
    if(!re) throw error(mfi::msg(128) <<
        "RegExp /" << pattern << '/' << opts <<
        " compilation failed at offset " << erroffset << ": " << errmsg);
}
//----------------------------------------------------------------------------
regexp::~regexp()
{
    if(re) ::pcre_free(re);
}
//----------------------------------------------------------------------------
bool regexp::match(const char *str, std::size_t len) const
{
    int rc = ::pcre_exec(re, nullptr, str, len, 0, 0, nullptr, 0);
    if(rc == PCRE_ERROR_NOMATCH) return false;
    if(rc < 0) throw error(mfi::msg(64) <<
        "Error while execute RegExp. Code: " << rc);
    return true;
}
//----------------------------------------------------------------------------
bool regexp::exec(const char *str, std::size_t len, match_results &matches) const
{
    int ovec[max_matches*3];
    int rc = ::pcre_exec(re, nullptr, str, len, 0, 0, ovec, max_matches*3);
    if(rc == PCRE_ERROR_NOMATCH) return false;
    if(rc > 0)
    {
        matches.clear();
        matches.reserve(rc);
        for(const int *p = ovec; rc--; p += 2)
            matches.push_back({str + *p, str + *(p+1)});
        return true;
    }
    if(rc == 0) throw error(mfi::msg(128) << "Too many matches. Only " <<
        max_matches << " matches allowed in this RegExp version");
    //assert(rc < 0);
    throw error(mfi::msg(64) << "Error while execute RegExp. Code: " << rc);
}
//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
class regexp::replacement
{
    struct entry
    {
        enum { plain, escape } type;
        const char *begin, *end;
        unsigned match_no;

        entry(const char *begin, const char *end)
            : type(plain), begin(begin), end(end) {}
        explicit entry(unsigned no)
            : type(escape), match_no(no) {}
    };
    std::vector<entry> segments;
public:
    replacement(const char * , const char * );
    void append(std::string & , const match_results & );
};
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
regexp::replacement::replacement(const char *begin, const char *end)
{
    segments.reserve(21);
    match_results matches;
    regexp re("\\$(\\d|\\$)");
    while(begin != end && re.exec(begin, end, matches))
    {
        auto m = matches.front();
        if(m.begin() != begin) segments.push_back({begin, m.begin()});
        auto m1 = matches[1];
        char ch = *m1.begin();
        segments.push_back(ch == '$' ?
            entry(m1.begin(), m1.begin() + 1) : entry(ch - '0'));
        begin = m.end();
    }
    if(begin != end) segments.push_back({begin, end});
}
//----------------------------------------------------------------------------
void regexp::replacement::append(
    std::string &res, const match_results &matches)
{
    for(const auto &s : segments)
    {
        if(s.type == entry::plain) res.append(s.begin, s.end);
        else
        {
            if(s.match_no >= matches.size()) throw error(mfi::msg(80) <<
                "No corresponding matching sub-expression for replacement"
                " reference $" << s.match_no);
            auto m = matches[s.match_no];
            res.append(m.begin(), m.end());
        }
    }
}
//----------------------------------------------------------------------------
std::string regexp::replace_first(
    const char *src_begin, const char *src_end,
    const char *repl_begin, const char *repl_end) const
{
    match_results matches;
    if(!exec(src_begin, src_end, matches)) return {src_begin, src_end};
    auto m = matches.front();
    std::string res(src_begin, m.begin());
    replacement rep(repl_begin, repl_end);
    rep.append(res, matches);
    res.append(m.end(), src_end);
    return res;
}
//----------------------------------------------------------------------------
std::string regexp::replace_all(
    const char *src_begin, const char *src_end,
    const char *repl_begin, const char *repl_end) const
{
    replacement rep(repl_begin, repl_end);
    match_results matches;
    const char *begin = src_begin;
    std::string res;
    for(;;)
    {
        if(!exec(begin, src_end, matches)) return res.append(begin, src_end);
        auto m = matches.front();
        res.append(begin, m.begin());
        rep.append(res, matches);
        if(m.end() == src_end) return res;
        begin = m.end();
    }
}
//----------------------------------------------------------------------------

} // namespace
