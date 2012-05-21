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
    bool IsImageTypeSupported(int type) const;
};

}

#endif
