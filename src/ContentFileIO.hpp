#ifndef __CONTENT_FILE_IO_HPP__
#define __CONTENT_FILE_IO_HPP__

#include "ContentIO.hpp"

namespace ExifAdapter
{

class ContentFileIO
    : public ContentIO
{
public:
    static libecap::shared_ptr<ContentFileIO> FromTemporaryFile();

    ~ContentFileIO();

    libecap::size_type Write(const libecap::Area& data);
    void Shift(libecap::size_type bytes);
    void ResetOffset();
    libecap::Area Read(
        libecap::size_type offset,
        libecap::size_type size);
    void ApplyFilter(libecap::shared_ptr<MetadataFilter> filter);
    uint64_t GetLength() const;

private:
    ContentFileIO(
        int fd,
        const std::string& filename);

    void OpenFile();
    void CloseFile();
    void RemoveFile();

    int fd;
    int offset;
    std::string filename;
};

}

#endif
