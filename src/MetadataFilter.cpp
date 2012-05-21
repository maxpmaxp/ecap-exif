#include "MetadataFilter.hpp"

#include <algorithm>

using namespace ExifAdapter;

//------------------------------------------------------------------------------
bool MetadataFilter::IsMimeTypeSupported(
    const std::string& mime_type) const
{
    if (std::find(mime_types.begin(), mime_types.end(), mime_type) ==
        mime_types.end())
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
unsigned MetadataFilter::DisableMimeTypes(
    const std::vector<std::string>& mime_types_to_exclude)
{
    for (std::vector<std::string>::const_iterator it = mime_types_to_exclude.begin();
         it != mime_types_to_exclude.end();
         ++it)
    {
        mime_types.erase(
            std::remove(mime_types.begin(), mime_types.end(), *it),
            mime_types.end());
    }

    return mime_types.size();
}
