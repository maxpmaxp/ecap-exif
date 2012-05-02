#include "MetadataFilterFactory.hpp"

#include "ExivMetadataFilter.hpp"

using namespace ExifAdapter;

//------------------------------------------------------------------------------
libecap::shared_ptr<MetadataFilter> MetadataFilterFactory::CreateFilter(
    const std::string& mime_type)
{
    libecap::shared_ptr<MetadataFilter> filter(new ExivMetadataFilter());
    if (filter->IsMimeTypeSupported(mime_type))
    {
        return filter;
    }
    return libecap::shared_ptr<MetadataFilter>();
}

//------------------------------------------------------------------------------
bool MetadataFilterFactory::IsMimeTypeSupported(
    const std::string& mime_type)
{
    if (mime_type.find("multipart/form-data") == 0)
    {
        return true;
    }

    libecap::shared_ptr<MetadataFilter> filter(new ExivMetadataFilter());
    return filter->IsMimeTypeSupported(mime_type);
}
