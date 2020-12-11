#ifndef __POSTPROC_CONTEXT_H
#define __POSTPROC_CONTEXT_H

#include<mfisoft/january/defs.h>
#include<mfisoft/january/fastmap.h>
#include<postproc/defs.h>
#include<postproc/exprs.h>
#include<postproc/session_constant_extractor.h>
#include<vector>
#include<string>
#include<list>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class context : private jan::non_copyable
{
    struct stub_const_extractor : public session_constant_extractor
    {
        stub_const_extractor() {} // only to get rid of "initializer"
                                  // in definition of const object
        std::string get(const std::string &name) const // override
            { throw_unknown_session_constant(name); }
    };
    static const stub_const_extractor stub_sce;

    typedef jan::fastmap<std::string, std::vector<string_literal> >
        named_lists_map;

    named_lists_map named_lists;
    const session_constant_extractor *sce;
public:
    context();
    explicit context(const session_constant_extractor & );
    ~context();

    std::string get_session_constant(const std::string &name) const
        { return sce->get(name); }
    const std::vector<string_literal> *
        get_named_list(const std::string & ) const;

    void add_list(const std::string & , const std::list<string_literal> & );
    context &operator=(const session_constant_extractor &e)
        { sce = &e; return *this; }
    const session_constant_extractor *get_sce() const { return sce; }
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
