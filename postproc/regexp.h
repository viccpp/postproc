#ifndef POSTPROC_REGEXP_H
#define POSTPROC_REGEXP_H

#include<postproc/defs.h>
#include<cstddef>
#include<utility>
#include<string>
#include<vector>
#include<pcre.h>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class regexp
{
    ::pcre *re;
    class replacement;
public:
    class match_result
    {
        const char *b, *e;
    public:
        typedef char value_type;
        typedef const value_type *const_iterator;
        typedef const_iterator iterator;
        typedef std::size_t size_type;
        match_result(const char *begin, const char *end) : b(begin), e(end) {}

        auto begin() const { return b; }
        auto cbegin() const { return begin(); }
        auto end() const { return e; }
        auto cend() const { return end(); }
        size_type length() const { return end() - begin(); }
    };
    typedef std::vector<match_result> match_results;
    struct error : public simple_exception
    {
        explicit error(const char *msg) : simple_exception(msg) {}
    };
    static const unsigned max_matches = 32;

    explicit regexp(const std::string & , const std::string & = {});
    regexp(regexp &&o) noexcept : re(o.re) { o.re = nullptr; }
    regexp(const regexp &) = delete;
    ~regexp();

    regexp &operator=(regexp &&o) noexcept { swap(o); return *this; }
    regexp &operator=(const regexp &) = delete;

    bool exec(const std::string &s, match_results &v) const
        { return exec(s.data(), s.length(), v); }
    bool exec(const char *begin, const char *end, match_results &v) const
        { return exec(begin, end - begin, v); }
    bool exec(const char * , std::size_t , match_results & ) const;

    bool match(const std::string &s) const
        { return match(s.data(), s.length()); }
    bool match(const char * , std::size_t ) const;

    std::string replace_first(
        const std::string &str, const std::string &repl) const
    {
        return replace_first(str.data(), &*str.end(),
                             repl.data(), &*repl.end());
    }
    std::string replace_first(const char * , const char * ,
                              const char * , const char * ) const;
    std::string replace_all(
        const std::string &str, const std::string &repl) const
    {
        return replace_all(str.data(), &*str.end(),
                           repl.data(), &*repl.end());
    }
    std::string replace_all(const char * , const char * ,
                            const char * , const char * ) const;

    void swap(regexp &o) noexcept { std::swap(re, o.re); }
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
