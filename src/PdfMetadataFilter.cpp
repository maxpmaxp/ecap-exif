#include "PdfMetadataFilter.hpp"

#include <cstdlib>

#include <errno.h>

#include <podofo/podofo.h>

#include "Log.hpp"

static const char* mime_types[] = {
    "application/pdf",
    NULL,
};


static void StripMetadata(PoDoFo::PdfMemDocument *document)
{
    PoDoFo::PdfObject *metadata = document->GetMetadata();
    if (metadata != NULL)
    {
        PoDoFo::PdfStream *stream = metadata->GetStream();
        if (stream != NULL)
        {
            stream->Set("");
        }
    }

    PoDoFo::PdfInfo *info = document->GetInfo();
    if (info != NULL)
    {
        info->SetAuthor(PoDoFo::PdfString());
        info->SetCreator(PoDoFo::PdfString());
        info->SetKeywords(PoDoFo::PdfString());
        info->SetSubject(PoDoFo::PdfString());
        info->SetTitle(PoDoFo::PdfString());
        info->SetProducer(PoDoFo::PdfString());
    }
}

using namespace ExifAdapter;

//------------------------------------------------------------------------------
PdfMetadataFilter::PdfMetadataFilter()
{
    Log(libecap::flXaction | libecap::ilDebug) <<
        "registered pdf metadata filter";
    PoDoFo::PdfError::EnableDebug(false);
}

//------------------------------------------------------------------------------
PdfMetadataFilter::~PdfMetadataFilter()
{
}

//------------------------------------------------------------------------------
void PdfMetadataFilter::ProcessFile(const std::string& path)
{
    Log(libecap::flXaction | libecap::ilDebug)
        << "applying filter to pdf file";

    try
    {
        PoDoFo::PdfMemDocument document(path.c_str());
        StripMetadata(&document);

        // PoDoFo can't read/write the same file
        // that's why we unlink the file - it is still available for reading
        // but new file will be created for writing
        if (unlink(path.c_str()) == -1)
        {
            std::string msg = "Failed to unlink temporary file: ";
            msg += strerror(errno);
            throw MetadataFilter::Exception(msg);
        }

        document.Write(path.c_str());
    }
    catch (PoDoFo::PdfError& e)
    {
        throw MetadataFilter::Exception(e.what());
    }
}

//------------------------------------------------------------------------------
void PdfMetadataFilter::ProcessMemory(
    uint8_t** buffer,
    int* size)
{
    Log(libecap::flXaction | libecap::ilDebug)
        << "applying filter to pdf memory area";

    try
    {
        PoDoFo::PdfMemDocument document;
        const char *data = reinterpret_cast<char*>(*buffer);
        document.Load(data, *size);
        StripMetadata(&document);

        PoDoFo::PdfRefCountedBuffer pdf_buffer;
        PoDoFo::PdfOutputDevice output_device(&pdf_buffer);
        document.Write(&output_device);

        if (*size < static_cast<int>(pdf_buffer.GetSize()))
        {
            uint8_t* new_buffer = static_cast<uint8_t*>(
                std::realloc(*buffer, pdf_buffer.GetSize()));
            if (new_buffer == NULL)
            {
                throw std::runtime_error("Failed to allocate memory");
            }
            (*buffer) = new_buffer;
        }
        (*size) = pdf_buffer.GetSize();

        memcpy(*buffer, pdf_buffer.GetBuffer(), *size);
    }
    catch (PoDoFo::PdfError& e)
    {
        throw MetadataFilter::Exception(e.what());
    }
}

//------------------------------------------------------------------------------
bool PdfMetadataFilter::IsMimeTypeSupported(
    const std::string& mime_type) const
{
    for (int i = 0; mime_types[i] != NULL; ++i)
    {
        if (mime_type == mime_types[i])
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
bool PdfMetadataFilter::CanProcess(const std::string& path) const
{
    try
    {
        PoDoFo::PdfMemDocument document(path.c_str());
    }
    catch (PoDoFo::PdfError& e)
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
bool PdfMetadataFilter::CanProcess(
    uint8_t* buffer,
    int size) const
{
    try
    {
        PoDoFo::PdfMemDocument document;
        const char* data = reinterpret_cast<char*>(buffer);
        document.Load(data, size);

    }
    catch (PoDoFo::PdfError& e)
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
bool PdfMetadataFilter::SupportsInMemoryProcessing() const
{
    return true;
}
