#include "MultipartContentIO.hpp"

#include <stdexcept>

#include <libecap/common/errors.h>

#include "ContentIOFactory.hpp"
#include "Log.hpp"
#include "MetadataFilterFactory.hpp"

static const std::string CRLF("\r\n");
static const std::string TRAILER("--\r\n");

using namespace ExifAdapter;

//------------------------------------------------------------------------------
class MultipartContentIO::FormData
{
public:
    FormData(
        const std::string& headers,
        libecap::shared_ptr<ContentIO> content,
        libecap::shared_ptr<MetadataFilter> filter)
        : headers(headers)
        , content(content)
        , filter(filter)
        {}

    libecap::size_type GetLength()
        {
            return headers.length() + content->GetLength();
        }

    const std::string headers;
    libecap::shared_ptr<ContentIO> content;
    libecap::shared_ptr<MetadataFilter> filter;
};

//------------------------------------------------------------------------------
MultipartContentIO::MultipartContentIO(
    const std::string& content_type,
    libecap::size_type content_length)
    : parse_state(LOOK_FOR_BOUNDARY)
    , content_length(content_length)
    , offset(0)
{
    const std::string boundary_key = "boundary=";
    size_t pos = content_type.find(boundary_key);
    if (pos == std::string::npos)
    {
        std::string msg = "boundary not found";
        Log(libecap::flXaction | libecap::ilNormal) << msg;
        throw libecap::TextException(msg);
    }
    pos += boundary_key.length();

    boundary = std::string("--") + content_type.substr(pos);
}

//------------------------------------------------------------------------------
MultipartContentIO::~MultipartContentIO()
{
}

//------------------------------------------------------------------------------
libecap::size_type MultipartContentIO::Write(const libecap::Area& data)
{
    // Log(libecap::flXaction | libecap::ilDebug) << "write multipart data";

    bool repeat = false;
    libecap::size_type processed = 0;

    do
    {
        repeat = false;
        // Log(libecap::flXaction | libecap::ilDebug)
        //     << " processed: " << processed;
        switch (parse_state)
        {
        case LOOK_FOR_BOUNDARY:
        {
            // Log(libecap::flXaction | libecap::ilDebug)
            //     << " look for boundary";
            int boundary_pos =
                FindString(data.start + processed,
                           data.size - processed,
                           boundary);
            if (boundary_pos >= 0)
            {
                // Log(libecap::flXaction | libecap::ilDebug)
                //     << "  boundary_pos: " << boundary_pos;

                processed += boundary_pos + boundary.size() + 2;
                parse_state = PARSE_HEADERS;
                repeat = true;
            }
            break;
        }
        case PARSE_HEADERS:
        {
            // Log(libecap::flXaction | libecap::ilDebug)
            //     << " parse headers";
            int crlfcrlf_pos = FindString(
                data.start + processed,
                data.size - processed,
                CRLF + CRLF);
            if (crlfcrlf_pos >= 0)
            {
                // Log(libecap::flXaction | libecap::ilDebug)
                //     << "  crlfcrlf_pos: " << crlfcrlf_pos;
                ParseHeaders(
                    data.start + processed,
                    crlfcrlf_pos + CRLF.size() * 2);
                processed += crlfcrlf_pos + CRLF.size() * 2;
                parse_state = PROCESS_DATA;
                repeat = true;
            }
            break;
        }
        case PROCESS_DATA:
        {
            // Log(libecap::flXaction | libecap::ilDebug)
            //     << " process data";

            int boundary_pos =
                FindString(data.start + processed,
                           data.size - processed,
                           boundary);
            if (boundary_pos >= 0)
            {
                // Log(libecap::flXaction | libecap::ilDebug)
                //     << "  boundary_pos: " << boundary_pos;
                AppendData(data.start + processed,
                           boundary_pos - CRLF.size());
                processed += boundary_pos;
                parse_state = LOOK_FOR_BOUNDARY;
                repeat = true;
            }
            else
            {
                // Log(libecap::flXaction | libecap::ilDebug)
                //     << "  append data";
                AppendData(data.start + processed,
                           data.size - processed);
                processed = data.size;
            }
            break;
        }
        };
    }
    while (repeat);

    // Log(libecap::flXaction | libecap::ilDebug)
    //     << " return processed: " << processed;

    return processed;
}

//------------------------------------------------------------------------------
void MultipartContentIO::Shift(libecap::size_type bytes)
{
    offset += bytes;
}

//------------------------------------------------------------------------------
void MultipartContentIO::ResetOffset()
{
    offset = 0;

    for (std::list<libecap::shared_ptr<FormData> >::const_iterator it =
             fields.begin();
         it != fields.end();
         ++it)
    {
        libecap::shared_ptr<FormData> form_data = *it;
        form_data->content->ResetOffset();
    }
}

//------------------------------------------------------------------------------
libecap::Area MultipartContentIO::Read(
    libecap::size_type offset,
    libecap::size_type size)
{
    libecap::size_type real_offset = offset + this->offset;



#define PROCESS_STRING(str)                         \
    if (real_offset < str.size())                   \
    {                                               \
        libecap::size_type real_size =              \
            std::min(                               \
                size,                               \
                str.size() - real_offset);          \
        return libecap::Area::FromTempBuffer(       \
            str.c_str() + real_offset,              \
            real_size);                             \
    }                                               \
    else                                            \
    {                                               \
        real_offset -= str.size();                  \
    }

    PROCESS_STRING(boundary);

    for (std::list<libecap::shared_ptr<FormData> >::const_iterator it =
             fields.begin();
         it != fields.end();
         ++it)
    {
        libecap::shared_ptr<FormData> form_data = *it;

        std::string headers_with_crlf = CRLF + form_data->headers;
        PROCESS_STRING(headers_with_crlf);

        if (real_offset < form_data->content->GetLength())
        {
            return form_data->content->Read(real_offset, size);
        }
        else
        {
            real_offset -= form_data->content->GetLength();
        }

        std::string boundary_with_crlf = CRLF + boundary;
        PROCESS_STRING(boundary_with_crlf);
    }

    PROCESS_STRING(TRAILER);

    return libecap::Area();
}

//------------------------------------------------------------------------------
void MultipartContentIO::ApplyFilter(
    libecap::shared_ptr<MetadataFilter> filter)
{
    for (std::list<libecap::shared_ptr<FormData> >::const_iterator it =
             fields.begin();
         it != fields.end();
         ++it)
    {
        libecap::shared_ptr<FormData> form_data = *it;
        if (form_data->filter)
        {
            try
            {
                form_data->content->ApplyFilter(form_data->filter);
            }
            catch (MetadataFilter::Exception& e)
            {
                Log(libecap::flXaction | libecap::ilDebug)
                    << "failed to apply filter " << e.what();
            }
        }
    }
}

//------------------------------------------------------------------------------
uint64_t MultipartContentIO::GetLength() const
{
    uint64_t length = 0;
    for (std::list<libecap::shared_ptr<FormData> >::const_iterator it =
             fields.begin();
         it != fields.end();
         ++it)
    {
        libecap::shared_ptr<FormData> form_data = *it;
        length += boundary.size() + CRLF.size();
        length += form_data->GetLength();
        length += CRLF.size();
    }
    length += boundary.size() + TRAILER.size();
    return length;
}

//------------------------------------------------------------------------------
int MultipartContentIO::FindString(
    const char* start,
    libecap::size_type size,
    const std::string& to_find) const
{
    if (size < to_find.size())
    {
        return -1;
    }

    for (unsigned i = 0; i < size - to_find.size(); ++i)
    {
        bool not_found = false;
        for (unsigned j = 0; j < to_find.size(); ++j)
        {
            if (start[i + j] != to_find[j])
            {
                not_found = true;
                break;
            }
        }
        if (not_found == false)
        {
            return i;
        }
    }

    return -1;
}

//------------------------------------------------------------------------------
void MultipartContentIO::ParseHeaders(
    const char* data,
    libecap::size_type size)
{
    std::string headers(data, size);
    std::string content_type = GetContentTypeFromHeaders(headers);
    libecap::shared_ptr<MetadataFilter> filter =
        MetadataFilterFactory::CreateFilter(content_type);
    fields.push_back(
        libecap::shared_ptr<FormData>(
            new FormData(
                headers,
                ContentIOFactory::CreateContentIO(
                    content_type,
                    content_length),
                filter)));
}

//------------------------------------------------------------------------------
void MultipartContentIO::AppendData(
    const char* data,
    libecap::size_type size)
{
    libecap::shared_ptr<FormData> formdata = fields.back();
    formdata->content->Write(libecap::Area(data, size));
}

//------------------------------------------------------------------------------
std::string MultipartContentIO::GetContentTypeFromHeaders(
    const std::string& headers)
{
    const std::string content_type_header("Content-Type: ");

    size_t header_pos = headers.find(content_type_header);
    if (header_pos == std::string::npos)
    {
        return "";
    }

    header_pos += content_type_header.size();

    size_t eol_pos = headers.find("\r\n", header_pos);
    if (eol_pos == std::string::npos)
    {
        return "";
    }

    return headers.substr(header_pos, eol_pos - header_pos);
}
