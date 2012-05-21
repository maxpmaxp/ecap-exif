#include "OpenXmlMetadataFilter.hpp"

#include <cstring>

#include "zip.h"

#include "Log.hpp"

static const char *core_xml_data = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><cp:coreProperties xmlns:cp=\"http://schemas.openxmlformats.org/package/2006/metadata/core-properties\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:dcterms=\"http://purl.org/dc/terms/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"></cp:coreProperties>";

using namespace ExifAdapter;

//------------------------------------------------------------------------------
OpenXmlMetadataFilter::OpenXmlMetadataFilter()
{
    mime_types.push_back("application/vnd.openxmlformats-officedocument.wordprocessingml.document");
    mime_types.push_back("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
    mime_types.push_back("application/vnd.openxmlformats-officedocument.presentationml.presentation");
}

//------------------------------------------------------------------------------
OpenXmlMetadataFilter::~OpenXmlMetadataFilter()
{
}

//------------------------------------------------------------------------------
void OpenXmlMetadataFilter::ProcessFile(const std::string& path)
{
    Log(libecap::flXaction | libecap::ilDebug)
        << "applying filter to openxml file";

    struct zip* archive = zip_open(path.c_str(), ZIP_CHECKCONS, NULL);
    if (archive == NULL)
    {
        throw MetadataFilter::Exception("Failed to open zip file");
    }

    if (!CanProcess(archive))
    {
        zip_close(archive);
        throw MetadataFilter::Exception("Not a openxml file");
    }

    int core_xml_index = zip_name_locate(
        archive,
        "docProps/core.xml",
        ZIP_FL_NOCASE);
    if (core_xml_index == -1)
    {
        zip_close(archive);
        throw MetadataFilter::Exception("Failed to locate core.xml");
    }

    struct zip_source *core_xml_source = zip_source_buffer(
        archive,
        core_xml_data,
        strlen(core_xml_data),
        0);
    if (core_xml_source == NULL)
    {
        zip_close(archive);
        throw MetadataFilter::Exception(
            "Failed to create new core.xml source");
    }

    if (zip_replace(
            archive,
            core_xml_index,
            core_xml_source) == -1)
    {
        zip_close(archive);
        throw MetadataFilter::Exception(
            "Failed to replace core.xml file");
    }

    if (zip_close(archive) == -1)
    {
        throw MetadataFilter::Exception(
            "Failed to write changes");
    }
}

//------------------------------------------------------------------------------
void OpenXmlMetadataFilter::ProcessMemory(
    uint8_t** buffer,
    int* size)
{
    (void) buffer;
    (void) size;
    throw MetadataFilter::Exception("filter can't process in memory");
}

//------------------------------------------------------------------------------
bool OpenXmlMetadataFilter::CanProcess(const std::string& path) const
{
    struct zip* archive = zip_open(path.c_str(), 0, NULL);
    if (archive == NULL)
    {
        return false;
    }

    bool can_process = CanProcess(archive);

    zip_close(archive);

    return can_process;
}

//------------------------------------------------------------------------------
bool OpenXmlMetadataFilter::CanProcess(
    uint8_t* buffer,
    int size) const
{
    (void) buffer;
    (void) size;
    return false;
}

//------------------------------------------------------------------------------
bool OpenXmlMetadataFilter::SupportsInMemoryProcessing() const
{
    return false;
}

//------------------------------------------------------------------------------
bool OpenXmlMetadataFilter::CanProcess(struct zip* archive) const
{
    if (zip_name_locate(
            archive,
            "[Content_Types].xml",
            ZIP_FL_NOCASE)  == -1)
    {
        return false;
    }

    if (zip_name_locate(
            archive,
            "docProps/core.xml",
            ZIP_FL_NOCASE) == -1)
    {
        return false;
    }

    return true;
}
