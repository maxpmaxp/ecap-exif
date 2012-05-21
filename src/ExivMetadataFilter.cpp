#include "ExivMetadataFilter.hpp"

#include <exiv2/exiv2.hpp>

#include "Log.hpp"

using namespace ExifAdapter;


//------------------------------------------------------------------------------
ExivMetadataFilter::ExivMetadataFilter()
{
    // http://dev.exiv2.org/projects/exiv2/wiki/Supported_image_formats
    mime_types.push_back("image/jpeg");
    mime_types.push_back("image/x-exv");
    mime_types.push_back("image/x-canon-cr2");
    mime_types.push_back("image/x-canon-crw");
    mime_types.push_back("image/tiff");
    mime_types.push_back("image/x-nikon-nef");
    mime_types.push_back("image/x-pentax-pef");
    mime_types.push_back("image/x-samsung-srw");
    mime_types.push_back("image/x-olympus-orf");
    mime_types.push_back("image/png");
    mime_types.push_back("image/pgf");
    mime_types.push_back("application/rdf+xml");
    mime_types.push_back("image/x-photoshop");
    mime_types.push_back("image/jp2");
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
bool ExivMetadataFilter::CanProcess(const std::string& path) const
{
    int image_type = Exiv2::ImageFactory::getType(path);
    return IsImageTypeSupported(image_type);
}

//------------------------------------------------------------------------------
bool ExivMetadataFilter::IsImageTypeSupported(int type) const
{
    if (type == Exiv2::ImageType::none)
    {
        return false;
    }

    std::string mime_type;

    switch (type)
    {
    case Exiv2::ImageType::jpeg:
        mime_type = "image/jpeg";
        break;
    case Exiv2::ImageType::exv:
        mime_type = "image/x-exv";
        break;
    case Exiv2::ImageType::cr2:
        mime_type = "image/x-canon-cr2";
        break;
    case Exiv2::ImageType::crw:
        mime_type = "image/x-canon-crw";
        break;
    case Exiv2::ImageType::tiff:
        mime_type = "image/tiff";
        break;
    case Exiv2::ImageType::orf:
        mime_type = "image/x-olympus-orf";
        break;
    case Exiv2::ImageType::png:
        mime_type = "image/png";
        break;
    case Exiv2::ImageType::pgf:
        mime_type = "image/pgf";
        break;
    case Exiv2::ImageType::xmp:
        mime_type = "application/rdf+xml";
        break;
    case Exiv2::ImageType::psd:
        mime_type = "image/x-photoshop";
        break;
    case Exiv2::ImageType::jp2:
        mime_type = "image/jp2";
        break;
    default:
        return false;
    }

    return IsMimeTypeSupported(mime_type);
}

//------------------------------------------------------------------------------
bool ExivMetadataFilter::CanProcess(
    uint8_t* buffer,
    int size) const
{
    int type = Exiv2::ImageFactory::getType(buffer, size);
    return IsImageTypeSupported(type);
}

//------------------------------------------------------------------------------
bool ExivMetadataFilter::SupportsInMemoryProcessing() const
{
    return true;
}
