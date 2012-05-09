#include "RuntimeMetadataFilter.hpp"

#include "ExivMetadataFilter.hpp"

using namespace ExifAdapter;

//------------------------------------------------------------------------------
RuntimeMetadataFilter::RuntimeMetadataFilter()
{
    filters.push_back(libecap::shared_ptr<MetadataFilter>(
                          new ExivMetadataFilter()));
}

//------------------------------------------------------------------------------
RuntimeMetadataFilter::~RuntimeMetadataFilter()
{
}

//------------------------------------------------------------------------------
void RuntimeMetadataFilter::ProcessFile(const std::string& path)
{
    for (std::list<libecap::shared_ptr<MetadataFilter> >::const_iterator it =
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

    Log(flXaction | ilDebug)
        << "no filter found";
}

//------------------------------------------------------------------------------
void RuntimeMetadataFilter::ProcessMemory(
    uint8_t** buffer,
    int* size)
{
    for (std::list<libecap::shared_ptr<MetadataFilter> >::const_iterator it =
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

    Log(flXaction | ilDebug)
        << "no filter found";
}

//------------------------------------------------------------------------------
bool RuntimeMetadataFilter::IsMimeTypeSupported(const std::string& mime_type) const
{
    for (std::list<libecap::shared_ptr<MetadataFilter> >::const_iterator it =
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
    for (std::list<libecap::shared_ptr<MetadataFilter> >::const_iterator it =
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
    for (std::list<libecap::shared_ptr<MetadataFilter> >::const_iterator it =
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
