#ifndef __MP3_METADATA_FILTER_HPP__
#define __MP3_METADATA_FILTER_HPP__

#include "MetadataFilter.hpp"

namespace ExifAdapter
{

class Mp3MetadataFilter
    : public MetadataFilter
{
public:
    Mp3MetadataFilter();
    ~Mp3MetadataFilter();

    void ProcessFile(const std::string& path);

    void ProcessMemory(
        uint8_t** buffer,
        int* size);

    bool CanProcess(const std::string& path) const;
    bool CanProcess(
        uint8_t* buffer,
        int size) const;

    bool SupportsInMemoryProcessing() const;
};

}

#endif
