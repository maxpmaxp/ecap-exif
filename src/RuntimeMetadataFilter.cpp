#include "RuntimeMetadataFilter.hpp"

#include "Log.hpp"
#include "MetadataFilterFactory.hpp"

using namespace ExifAdapter;

//------------------------------------------------------------------------------
RuntimeMetadataFilter::RuntimeMetadataFilter()
{
}

//------------------------------------------------------------------------------
RuntimeMetadataFilter::~RuntimeMetadataFilter()
{
}

//------------------------------------------------------------------------------
void RuntimeMetadataFilter::ProcessFile(const std::string& path)
{
    const MetadataFilterFactory::MetadataFilterList& filters =
        MetadataFilterFactory::GetFilters();

    for (MetadataFilterFactory::MetadataFilterList::const_iterator it =
             filters.begin();
         it != filters.end();
         ++it)
    {
        libecap::shared_ptr<MetadataFilter> filter = *it;
        if (filter->CanProcess(path))
        {
            filter->ProcessFile(path);
            return;
        }
    }

    Log(libecap::flXaction | libecap::ilDebug)
        << "no filter found";
}

//------------------------------------------------------------------------------
void RuntimeMetadataFilter::ProcessMemory(
    uint8_t** buffer,
    int* size)
{
    const MetadataFilterFactory::MetadataFilterList& filters =
        MetadataFilterFactory::GetFilters();

    for (MetadataFilterFactory::MetadataFilterList::const_iterator it =
             filters.begin();
         it != filters.end();
         ++it)
    {
        libecap::shared_ptr<MetadataFilter> filter = *it;
        if (filter->CanProcess(*buffer, *size))
        {
            filter->ProcessMemory(buffer, size);
            return;
        }
    }

    Log(libecap::flXaction | libecap::ilDebug)
        << "no filter found";
}

//------------------------------------------------------------------------------
bool RuntimeMetadataFilter::IsMimeTypeSupported(const std::string& mime_type) const
{
    const MetadataFilterFactory::MetadataFilterList& filters =
        MetadataFilterFactory::GetFilters();

    for (MetadataFilterFactory::MetadataFilterList::const_iterator it =
             filters.begin();
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
bool RuntimeMetadataFilter::CanProcess(const std::string& path) const
{
    const MetadataFilterFactory::MetadataFilterList& filters =
        MetadataFilterFactory::GetFilters();

    for (MetadataFilterFactory::MetadataFilterList::const_iterator it =
             filters.begin();
         it != filters.end();
         ++it)
    {
        libecap::shared_ptr<MetadataFilter> filter = *it;
        if (filter->CanProcess(path))
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
bool RuntimeMetadataFilter::CanProcess(
    uint8_t* buffer,
    int size) const
{
    const MetadataFilterFactory::MetadataFilterList& filters =
        MetadataFilterFactory::GetFilters();

    for (MetadataFilterFactory::MetadataFilterList::const_iterator it =
             filters.begin();
         it != filters.end();
         ++it)
    {
        libecap::shared_ptr<MetadataFilter> filter = *it;
        if (filter->CanProcess(buffer, size))
        {
            return true;
        }
    }

    return false;
}
