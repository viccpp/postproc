#ifndef POSTPROC_MAPPING_H
#define POSTPROC_MAPPING_H

#include<postproc/defs.h>
#include<postproc/exprs.h>
#include<unordered_map>
#include<memory>
#include<list>

namespace postproc {

//////////////////////////////////////////////////////////////////////////////
class mapping
{
    std::unordered_map<std::string, const expression*> m;
public:
    typedef std::list<std::pair<
        std::list<string_literal>,
        std::unique_ptr<const expression>
    >> list;

    explicit mapping(list );
    mapping(const mapping &) = delete;
    mapping &operator=(const mapping &) = delete;
    ~mapping();

    const expression *find(const std::string &key) const
    {
        auto it = m.find(key);
        return it != m.end() ? it->second : nullptr;
    }
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
