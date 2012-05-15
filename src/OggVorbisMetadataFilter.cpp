#include "OggVorbisMetadataFilter.hpp"

#include <vorbisfile.h>

#include "Log.hpp"

static const char* mime_types[] = {
    "audio/ogg",
    NULL,
};

using namespace ExifAdapter;

//------------------------------------------------------------------------------
OggVorbisMetadataFilter::OggVorbisMetadataFilter()
{
    Log(libecap::flXaction | libecap::ilDebug) <<
        "registered ogg metadata filter";
}

//------------------------------------------------------------------------------
OggVorbisMetadataFilter::~OggVorbisMetadataFilter()
{
}

//------------------------------------------------------------------------------
void OggVorbisMetadataFilter::ProcessFile(const std::string& path)
{
    Log(libecap::flXaction | libecap::ilDebug)
        << "applying filter to ogg file";

    if (!CanProcess(path))
    {
        throw MetadataFilter::Exception("Not a OGG file");
    }

    TagLib::Ogg::Vorbis::File file(path.c_str());
    if (!file.isValid())
    {
        throw MetadataFilter::Exception("Failed to read file");
    }

    TagLib::Ogg::XiphComment *tag = file.tag();
    if (!tag)
    {
        throw MetadataFilter::Exception("Failed to read tag");
    }

    std::list<TagLib::String> fields_to_remove;

    const TagLib::Ogg::FieldListMap& field_list_map = tag->fieldListMap();
    for (TagLib::Ogg::FieldListMap::ConstIterator it = field_list_map.begin();
         it != field_list_map.end();
         ++it)
    {
        fields_to_remove.push_back(it->first);
    }

    for (std::list<TagLib::String>::const_iterator it = fields_to_remove.begin();
         it != fields_to_remove.end();
         ++it)
    {
        tag->removeField(*it);
    }

    if (!file.save())
    {
        throw MetadataFilter::Exception("Failed to save file");
    }
}

//------------------------------------------------------------------------------
void OggVorbisMetadataFilter::ProcessMemory(
    uint8_t** buffer,
    int* size)
{
    (void) buffer;
    (void) size;
    throw MetadataFilter::Exception("filter can't process in memory");
}

//------------------------------------------------------------------------------
bool OggVorbisMetadataFilter::IsMimeTypeSupported(
    const std::string& mime_type) const
{
    for (int i = 0; mime_types[i] != NULL; ++i)
    {
        if (mime_type == mime_types[i])
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
bool OggVorbisMetadataFilter::CanProcess(const std::string& path) const
{
    TagLib::Ogg::Vorbis::File file(path.c_str());
    if (!file.isValid())
    {
        std::cout << "not valid" << std::endl;
        return false;
    }

    TagLib::Ogg::XiphComment *tag = file.tag();
    if (!tag)
    {
        std::cout << "no tag" << std::endl;
        return false;
    }

    if (tag->isEmpty())
    {
        std::cout << "empty tag" << std::endl;
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
bool OggVorbisMetadataFilter::CanProcess(
    uint8_t* buffer,
    int size) const
{
    (void) buffer;
    (void) size;
    return false;
}

//------------------------------------------------------------------------------
bool OggVorbisMetadataFilter::SupportsInMemoryProcessing() const
{
    return false;
}
