#ifndef __MP4_METADATA_FILTER_HPP__
#define __MP4_METADATA_FILTER_HPP__

#include "MetadataFilter.hpp"

namespace ExifAdapter
{

class Mp4MetadataFilter
    : public MetadataFilter
{
public:
    Mp4MetadataFilter();
    ~Mp4MetadataFilter();

    void ProcessFile(const std::string& path);

    void ProcessMemory(
        uint8_t** buffer,
        int* size);

    bool CanProcess(const std::string& path) const;
    bool CanProcess(
        uint8_t* buffer,
        int size) const;

    bool SupportsInMemoryProcessing() const;

private:
    bool IsFtypSupported(const char* atom) const;
    void ProcessMoov(
        uint8_t* buffer,
        uint64_t size);
};

}

#endif
