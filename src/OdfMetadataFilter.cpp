#include "OdfMetadataFilter.hpp"

#include <cstring>

#include "zip.h"

#include "Log.hpp"

static const char *meta_xml_data = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><office:document-meta xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" xmlns:ooo=\"http://openoffice.org/2004/office\" xmlns:grddl=\"http://www.w3.org/2003/g/data-view#\" office:version=\"1.2\"></office:document-meta>";

using namespace ExifAdapter;

//------------------------------------------------------------------------------
OdfMetadataFilter::OdfMetadataFilter()
{
    mime_types.push_back("application/vnd.oasis.opendocument.text");
    mime_types.push_back("application/vnd.oasis.opendocument.presentation");
    mime_types.push_back("application/vnd.oasis.opendocument.spreadsheet");
    mime_types.push_back("application/vnd.oasis.opendocument.graphics");
}

//------------------------------------------------------------------------------
OdfMetadataFilter::~OdfMetadataFilter()
{
}

//------------------------------------------------------------------------------
void OdfMetadataFilter::ProcessFile(const std::string& path)
{
    Log(libecap::flXaction | libecap::ilDebug)
        << "applying filter to odf file";

    struct zip* archive = zip_open(path.c_str(), ZIP_CHECKCONS, NULL);
    if (archive == NULL)
    {
        throw MetadataFilter::Exception("Failed to open zip file");
    }

    if (!CanProcess(archive))
    {
        zip_close(archive);
        throw MetadataFilter::Exception("Not a odf file");
    }

    int meta_xml_index = zip_name_locate(archive, "meta.xml", ZIP_FL_NOCASE);
    if (meta_xml_index == -1)
    {
        zip_close(archive);
        throw MetadataFilter::Exception("Failed to locate meta.xml");
    }

    struct zip_source *meta_xml_source = zip_source_buffer(
        archive,
        meta_xml_data,
        strlen(meta_xml_data),
        0);
    if (meta_xml_source == NULL)
    {
        zip_close(archive);
        throw MetadataFilter::Exception(
            "Failed to create new meta.xml source");
    }

    if (zip_replace(
            archive,
            meta_xml_index,
            meta_xml_source) == -1)
    {
        zip_close(archive);
        throw MetadataFilter::Exception(
            "Failed to replace meta.xml file");
    }

    if (zip_close(archive) == -1)
    {
        throw MetadataFilter::Exception(
            "Failed to write changes");
    }
}

//------------------------------------------------------------------------------
void OdfMetadataFilter::ProcessMemory(
    uint8_t** buffer,
    int* size)
{
    (void) buffer;
    (void) size;
    throw MetadataFilter::Exception("filter can't process in memory");
}

//------------------------------------------------------------------------------
bool OdfMetadataFilter::CanProcess(const std::string& path) const
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
bool OdfMetadataFilter::CanProcess(
    uint8_t* buffer,
    int size) const
{
    (void) buffer;
    (void) size;
    return false;
}

//------------------------------------------------------------------------------
bool OdfMetadataFilter::SupportsInMemoryProcessing() const
{
    return false;
}

//------------------------------------------------------------------------------
bool OdfMetadataFilter::CanProcess(struct zip* archive) const
{
    struct zip_file *mimetype_file = zip_fopen(
        archive,
        "mimetype",
        ZIP_FL_NOCASE);
    if (mimetype_file == NULL)
    {
        return false;
    }

    const unsigned buffer_size = 100;
    char buffer[buffer_size] = {0};
    int bytes_read = zip_fread(mimetype_file, buffer, buffer_size);
    if (bytes_read == -1)
    {
        zip_fclose(mimetype_file);
        return false;
    }

    std::string mimetype(buffer);
    if (!IsMimeTypeSupported(mimetype))
    {
        zip_fclose(mimetype_file);
        return false;
    }

    zip_fclose(mimetype_file);

    return true;
}
