#include "ExivMetadataFilter.hpp"

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
bool ExivMetadataFilter::ProcessFile(const std::string& path)
{
    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(path);
        image->clearMetadata();
        image->writeMetadata();
        return true;
    }
    catch (Exiv2::AnyError& e)
    {
        return false;
    }
}
