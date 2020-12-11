#ifndef __POSTPROC_MAPPING_H
#define __POSTPROC_MAPPING_H

#include<postproc/defs.h>
#include<postproc/exprs.h>
#include<mfisoft/january/fastmap.h>
#include<list>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class mapping : private jan::non_copyable
{
    typedef jan::fastmap<std::string, const expression*> map_t;
    map_t m;
public:
    typedef std::list<std::pair<std::list<string_literal>,const expression *> >
        list;

    explicit mapping(list & );
    ~mapping();

    const expression *find(const std::string &key) const
    {
        map_t::const_iterator it = m.find(key);
        return it != m.end() ? it->second : nullptr;
    }
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
