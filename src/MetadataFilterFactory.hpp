#ifndef __METADATA_FILTER_FACTORY_HPP__
#define __METADATA_FILTER_FACTORY_HPP__

#include <libecap/common/memory.h>

#include "MetadataFilter.hpp"

namespace ExifAdapter
{

class MetadataFilterFactory
{
public:
    static libecap::shared_ptr<MetadataFilter> CreateFilter(
        const std::string& mime_type);
    static bool IsMimeTypeSupported(const std::string& mime_type);
};

}

#endif
