#include "Mp3MetadataFilter.hpp"

#include <fstream>

#include <mpegfile.h>

#include "Log.hpp"

using namespace ExifAdapter;

//------------------------------------------------------------------------------
Mp3MetadataFilter::Mp3MetadataFilter()
{
    mime_types.push_back("audio/mpeg");
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
bool Mp3MetadataFilter::CanProcess(const std::string& path) const
{
    std::ifstream file(path.c_str());
    if (!file.is_open())
    {
        return false;
    }

    uint8_t header[3] = {0};
    file.read(reinterpret_cast<char *>(header), 3);
    if (file.gcount() != 3)
    {
        return false;
    }

    if (header[0] == 'I' &&
        header[1] == 'D' &&
        header[2] == '3')
    {
        // assume that ID3 is used mostly with MP3
        // of course it's not true, but better try to process another file
        return true;
    }

    if (header[0] == 0xFF &&
        (header[1] & 0xF0) == 0xF0)
    {
        file.seekg(-128, std::fstream::end);
        if (!file.good())
        {
            return false;
        }

        file.read(reinterpret_cast<char *>(header), 3);
        if (file.gcount() != 3)
        {
            return false;
        }

        if (header[0] == 'T' &&
            header[1] == 'A' &&
            header[2] == 'G')
        {
            // mp3 with idv1
            return true;
        }
    }

    return false;
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
