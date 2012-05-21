#ifndef __OPEN_XML_METADATA_FILTER_HPP__
#define __OPEN_XML_METADATA_FILTER_HPP__

#include "MetadataFilter.hpp"

struct zip;

namespace ExifAdapter
{

class OpenXmlMetadataFilter
    : public MetadataFilter
{
public:
    OpenXmlMetadataFilter();
    ~OpenXmlMetadataFilter();

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
