#include "MetadataFilterFactory.hpp"

#include "ExivMetadataFilter.hpp"
#include "Log.hpp"
#include "Mp4MetadataFilter.hpp"
#include "RuntimeMetadataFilter.hpp"

using namespace ExifAdapter;

//------------------------------------------------------------------------------
libecap::shared_ptr<MetadataFilter> MetadataFilterFactory::CreateFilter(
    const std::string& mime_type)
{
    if (mime_type.find("application/octet-stream") == 0)
    {
        Log(libecap::flXaction | libecap::ilDebug)
            << "creating runtime metadata filter for "
            << "application/octet-stream mime type";
        libecap::shared_ptr<MetadataFilter> filter(
            new RuntimeMetadataFilter());
        return filter;
    }

    libecap::shared_ptr<MetadataFilter> filter(new ExivMetadataFilter());
    if (filter->IsMimeTypeSupported(mime_type))
    {
        return filter;
    }

    libecap::shared_ptr<MetadataFilter> mp4filter(new Mp4MetadataFilter());
    if (mp4filter->IsMimeTypeSupported(mime_type))
    {
        return mp4filter;
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
    else if (mime_type.find("application/octet-stream") == 0)
    {
        return true;
    }

    libecap::shared_ptr<MetadataFilter> filter(new ExivMetadataFilter());
    if (filter->IsMimeTypeSupported(mime_type))
    {
        return true;
    }

    libecap::shared_ptr<MetadataFilter> mp4filter(new Mp4MetadataFilter());
    if (mp4filter->IsMimeTypeSupported(mime_type))
    {
        return true;
    }

    return false;
}
