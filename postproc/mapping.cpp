#include<postproc/mapping.h>
#include<__vic/string_buffer.h>
#include<unordered_set>

namespace postproc {

//----------------------------------------------------------------------------
mapping::mapping(list mset)
{
    for(auto &in : mset)
        for(auto &key : in.first)
            if(!m.emplace(key, in.second.get()).second)
                throw invalid_function_arg(__vic::msg(64) <<
                    "Duplicate key value \"" << key << "\" in mapping");
    for(auto &in : mset)
        in.second.release();
}
//----------------------------------------------------------------------------
mapping::~mapping()
{
    // Delete each pointer only once
    std::unordered_set<const expression*> s;
    for(auto &ex : m)
        if(s.insert(ex.second).second) delete ex.second;
}
//----------------------------------------------------------------------------

} // namespace
