#include "MetadataFilterFactory.hpp"

#include "ExivMetadataFilter.hpp"
#include "Log.hpp"
#include "Mp3MetadataFilter.hpp"
#include "Mp4MetadataFilter.hpp"
#include "OdfMetadataFilter.hpp"
#include "OggVorbisMetadataFilter.hpp"
#include "PdfMetadataFilter.hpp"
#include "RuntimeMetadataFilter.hpp"

class FilterRegistry
{
public:
    void RegisterFilter(
        libecap::shared_ptr<ExifAdapter::MetadataFilter> filter)
        {
            filters.push_back(filter);
        }

    const ExifAdapter::MetadataFilterFactory::MetadataFilterList& GetFilters()
        {
            return filters;
        }

private:
    ExifAdapter::MetadataFilterFactory::MetadataFilterList filters;
};

static FilterRegistry* filter_registry = NULL;

static FilterRegistry* GetFilterRegistry()
{
    if (filter_registry == NULL)
    {
        filter_registry = new FilterRegistry();

#define REGISTER_FILTER(filter)                               \
        filter_registry->RegisterFilter(                      \
            libecap::shared_ptr<ExifAdapter::MetadataFilter>( \
                new filter()));

        REGISTER_FILTER(ExifAdapter::ExivMetadataFilter);
        REGISTER_FILTER(ExifAdapter::Mp4MetadataFilter);
        REGISTER_FILTER(ExifAdapter::Mp3MetadataFilter);
        REGISTER_FILTER(ExifAdapter::OggVorbisMetadataFilter);
        REGISTER_FILTER(ExifAdapter::PdfMetadataFilter);
        REGISTER_FILTER(ExifAdapter::OdfMetadataFilter);
    }

    return filter_registry;
}

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

    const MetadataFilterList& filters = GetFilterRegistry()->GetFilters();
    for (MetadataFilterList::const_iterator it = filters.begin();
         it != filters.end();
         ++it)
    {
        libecap::shared_ptr<MetadataFilter> filter = *it;
        if (filter->IsMimeTypeSupported(mime_type))
        {
            return filter;
        }
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

    const MetadataFilterList& filters = GetFilterRegistry()->GetFilters();
    for (MetadataFilterList::const_iterator it = filters.begin();
         it != filters.end();
         ++it)
    {
        libecap::shared_ptr<MetadataFilter> filter = *it;
        if (filter->IsMimeTypeSupported(mime_type))
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
const MetadataFilterFactory::MetadataFilterList& MetadataFilterFactory::GetFilters()
{
    return GetFilterRegistry()->GetFilters();
}
