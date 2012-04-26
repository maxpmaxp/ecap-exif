#ifndef __METADATA_FILTER_HPP__
#define __METADATA_FILTER_HPP__

#include <string>

namespace ExifAdapter
{

class MetadataFilter
{
public:
    virtual bool ProcessFile(const std::string& path) = 0;
};

}

#endif
