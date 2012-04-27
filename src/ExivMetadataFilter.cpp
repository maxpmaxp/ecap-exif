#include "ExivMetadataFilter.hpp"

#include <stdexcept>

#include <exiv2/error.hpp>
#include <exiv2/image.hpp>

using namespace ExifAdapter;

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
        throw std::runtime_error(e.what());
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
        throw std::runtime_error(e.what());
    }
}
