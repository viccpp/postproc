#include<postproc/mapping.h>
#include<mfisoft/january/string_buffer.h>

namespace postproc {

//----------------------------------------------------------------------------
mapping::mapping(list &mset)
{
    for(list::iterator it = mset.begin(); it != mset.end(); ++it)
        for(list::value_type::first_type::const_iterator
            key = it->first.begin(); key != it->first.end(); ++key)
        {
            if(!m.insert(std::make_pair(*key, it->second)).second)
                throw invalid_function_arg(jan::msg(64) <<
                    "Duplicate key value \"" << *key << "\" in mapping");
        }
    mset.clear();
}
//----------------------------------------------------------------------------
mapping::~mapping()
{
    // Delete each pointer only once
    jan::fastset<const expression*> s;
    for(map_t::const_iterator it = m.begin(); it != m.end(); ++it)
        if(s.insert(it->second).second) delete it->second;
}
//----------------------------------------------------------------------------

} // namespace
