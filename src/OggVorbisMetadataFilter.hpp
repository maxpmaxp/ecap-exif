#ifndef __OGG_VORBIS_METADATA_FILTER_HPP__
#define __OGG_VORBIS_METADATA_FILTER_HPP__

#include "MetadataFilter.hpp"

namespace ExifAdapter
{

class OggVorbisMetadataFilter
    : public MetadataFilter
{
public:
    OggVorbisMetadataFilter();
    ~OggVorbisMetadataFilter();

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
