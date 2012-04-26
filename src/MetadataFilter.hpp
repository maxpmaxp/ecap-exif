#ifndef __METADATA_FILTER_HPP__
#define __METADATA_FILTER_HPP__

#include <stdint.h>
#include <string>

namespace ExifAdapter
{

class MetadataFilter
{
public:
    virtual ~MetadataFilter() {};

    virtual bool ProcessFile(const std::string& path) = 0;

    // can realloc buffer
    virtual bool ProcessMemory(
        uint8_t** buffer,
        int* size) = 0;
};

}

#endif
