#ifndef __PDF_METADATA_FILTER_HPP__
#define __PDF_METADATA_FILTER_HPP__

#include "MetadataFilter.hpp"

namespace ExifAdapter
{

class PdfMetadataFilter
    : public MetadataFilter
{
public:
    PdfMetadataFilter();
    ~PdfMetadataFilter();

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
