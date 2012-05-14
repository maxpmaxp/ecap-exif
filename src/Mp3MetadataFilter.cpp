#include "Mp3MetadataFilter.hpp"

#include <mpegfile.h>

#include "Log.hpp"

static const char* mime_types[] = {
    "audio/mpeg",
    NULL,
};

using namespace ExifAdapter;

//------------------------------------------------------------------------------
Mp3MetadataFilter::Mp3MetadataFilter()
{
    Log(libecap::flXaction | libecap::ilDebug) <<
        "registered mp3 metadata filter";
}

//------------------------------------------------------------------------------
Mp3MetadataFilter::~Mp3MetadataFilter()
{
}

//------------------------------------------------------------------------------
void Mp3MetadataFilter::ProcessFile(const std::string& path)
{
    Log(libecap::flXaction | libecap::ilDebug)
        << "applying filter to mp3 file";

    if (!CanProcess(path))
    {
        throw MetadataFilter::Exception("Not a MP3 file");
    }

    TagLib::MPEG::File file(path.c_str());
    if (!file.isValid())
    {
        throw MetadataFilter::Exception("Failed to read file");
    }

    if (!file.strip())
    {
        throw MetadataFilter::Exception("Failed to strip tags");
    }

    if (!file.save())
    {
        throw MetadataFilter::Exception("Failed to save file");
    }
}

//------------------------------------------------------------------------------
void Mp3MetadataFilter::ProcessMemory(
    uint8_t** buffer,
    int* size)
{
    (void) buffer;
    (void) size;
    throw MetadataFilter::Exception("filter can't process in memory");
}

//------------------------------------------------------------------------------
bool Mp3MetadataFilter::IsMimeTypeSupported(
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
bool Mp3MetadataFilter::CanProcess(const std::string& path) const
{
    TagLib::MPEG::File file(path.c_str());
    if (!file.isValid())
    {
        return false;
    }

    if (file.firstFrameOffset() == -1 ||
        file.lastFrameOffset() == -1)
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
bool Mp3MetadataFilter::CanProcess(
    uint8_t* buffer,
    int size) const
{
    (void) buffer;
    (void) size;
    return false;
}

//------------------------------------------------------------------------------
bool Mp3MetadataFilter::SupportsInMemoryProcessing() const
{
    return false;
}
