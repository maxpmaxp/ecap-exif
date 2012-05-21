#ifndef __ODF_METADATA_FILTER_HPP__
#define __ODF_METADATA_FILTER_HPP__

#include "MetadataFilter.hpp"

struct zip;

namespace ExifAdapter
{

class OdfMetadataFilter
    : public MetadataFilter
{
public:
    OdfMetadataFilter();
    ~OdfMetadataFilter();

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
    bool CanProcess(struct zip* archive) const;
};

}

#endif
