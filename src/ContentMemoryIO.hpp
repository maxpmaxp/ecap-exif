#ifndef __CONTENT_MEMORY_IO_HPP__
#define __CONTENT_MEMORY_IO_HPP__

#include "ContentIO.hpp"

namespace ExifAdapter
{

class ContentMemoryIO
    : public ContentIO
{
public:
    ContentMemoryIO(libecap::size_type expected_size);
    ~ContentMemoryIO();

    libecap::size_type Write(const libecap::Area& data);
    void Shift(libecap::size_type bytes);
    void ResetOffset();
    libecap::Area Read(
        libecap::size_type offset,
        libecap::size_type size);
    void ApplyFilter(libecap::shared_ptr<MetadataFilter> filter);
    uint64_t GetLength() const;

private:
    uint8_t* buffer;
    libecap::size_type size;
    libecap::size_type offset;
    libecap::size_type written;
};

}

#endif
