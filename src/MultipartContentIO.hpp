#ifndef __MULTIPART_CONTENT_IO_HPP__
#define __MULTIPART_CONTENT_IO_HPP__

#include <list>

#include "ContentIO.hpp"

namespace ExifAdapter
{

class MultipartContentIO
    : public ContentIO
{
public:
    MultipartContentIO(
        const std::string& content_type,
        libecap::size_type content_length);
    ~MultipartContentIO();

    libecap::size_type Write(const libecap::Area& data);
    void Shift(libecap::size_type bytes);
    void ResetOffset();
    libecap::Area Read(
        libecap::size_type offset,
        libecap::size_type size);
    void ApplyFilter(libecap::shared_ptr<MetadataFilter> filter);
    uint64_t GetLength() const;

private:
    enum ParseState
    {
        LOOK_FOR_BOUNDARY,
        PARSE_HEADERS,
        PROCESS_DATA,
    };

    class FormData;

    int FindString(
        const char* start,
        libecap::size_type size,
        const std::string& to_find) const;
    void ParseHeaders(
        const char* start,
        libecap::size_type size);
    void AppendData(
        const char* start,
        libecap::size_type size);
    std::string GetContentTypeFromHeaders(const std::string& headers);

    ParseState parse_state;
    std::string boundary;
    libecap::size_type content_length;

    std::list<libecap::shared_ptr<FormData> > fields;

    libecap::size_type offset;
};

}

#endif
