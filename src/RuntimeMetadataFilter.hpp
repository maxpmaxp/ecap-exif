#ifndef __RUNTIME_METADATA_FILTER_HPP__
#define __RUNTIME_METADATA_FILTER_HPP__

#include <list>

#include <libecap/common/memory.h>

#include "MetadataFilter.hpp"

namespace ExifAdapter
{

class RuntimeMetadataFilter
    : public MetadataFilter
{
public:
    RuntimeMetadataFilter();
    ~RuntimeMetadataFilter();

    void ProcessFile(const std::string& path);

    void ProcessMemory(
        uint8_t** buffer,
        int* size);

    bool IsMimeTypeSupported(const std::string& mime_type) const;

    bool CanProcess(const std::string& path) const;
    bool CanProcess(
        uint8_t* buffer,
        int size) const;

    bool SupportsInMemoryProcessing() const;
};

}

#endif
