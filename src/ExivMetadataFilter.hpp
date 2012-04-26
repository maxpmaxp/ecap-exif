#ifndef __EXIV_METADATA_FILTER_HPP__
#define __EXIV_METADATA_FILTER_HPP__

#include "MetadataFilter.hpp"

namespace ExifAdapter
{

class ExivMetadataFilter
    : public MetadataFilter
{
public:
    ExivMetadataFilter();
    ~ExivMetadataFilter();

    bool ProcessFile(const std::string& path);
};

}

#endif
