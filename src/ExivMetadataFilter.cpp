#include "ExivMetadataFilter.hpp"

#include <exiv2/error.hpp>
#include <exiv2/image.hpp>

using namespace ExifAdapter;

// http://dev.exiv2.org/projects/exiv2/wiki/Supported_image_formats
static const char* mime_types[] = {
    "image/jpeg",
//    "image/x-exv",
//    "image/x-canon-cr2",
//    "image/x-canon-crw",
//    "image/tiff",
//    "image/x-nikon-nef",
//    "image/x-pentax-pef",
//    "image/x-samsung-srw",
//    "image/x-olympus-orf",
    "image/png",
//    "image/pgf",
//    "application/postscript",
//    "application/rdf+xml",
//    "image/x-photoshop",
//    "image/targa",
//    "image/jp2",
    NULL,
};

//------------------------------------------------------------------------------
ExivMetadataFilter::ExivMetadataFilter()
{
}

//------------------------------------------------------------------------------
ExivMetadataFilter::~ExivMetadataFilter()
{
}

//------------------------------------------------------------------------------
void ExivMetadataFilter::ProcessFile(const std::string& path)
{
    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(path);
        image->clearMetadata();
        image->writeMetadata();
    }
    catch (Exiv2::AnyError& e)
    {
        throw MetadataFilter::Exception(e.what());
    }
}

//------------------------------------------------------------------------------
void ExivMetadataFilter::ProcessMemory(
    uint8_t** buffer,
    int* size)
{
    try
    {
        Exiv2::Image::AutoPtr image =
            Exiv2::ImageFactory::open(*buffer, *size);
        image->clearMetadata();
        image->writeMetadata();

        // now exiv2 made a copy of data upon write call
        // and we can safely use buffer

        Exiv2::BasicIo& io = image->io();
        if (*size < io.size())
        {
            uint8_t* new_buffer = static_cast<uint8_t*>(
                std::realloc(*buffer, io.size()));
            if (new_buffer == NULL)
            {
                throw std::runtime_error("Failed to allocate memory");
            }
            (*buffer) = new_buffer;
        }
        (*size) = io.size();

        std::memcpy(*buffer, io.mmap(), io.size());
    }
    catch (Exiv2::AnyError& e)
    {
        throw MetadataFilter::Exception(e.what());
    }
}

//------------------------------------------------------------------------------
bool ExivMetadataFilter::IsMimeTypeSupported(
    const std::string& mime_type)
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
