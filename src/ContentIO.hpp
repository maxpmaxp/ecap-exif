#ifndef __CONTENT_IO_HPP__
#define __CONTENT_IO_HPP__

#include <stdint.h>

#include <libecap/common/area.h>

#include "MetadataFilter.hpp"

namespace ExifAdapter
{

class ContentIO
{
public:
    virtual ~ContentIO() {};

    virtual libecap::size_type Write(const libecap::Area& data) = 0;

    virtual void Shift(libecap::size_type bytes) = 0;

    virtual void ResetOffset() = 0;

    virtual libecap::Area Read(
        libecap::size_type offset,
        libecap::size_type size) = 0;

    // throws if failed to apply filter
    virtual void ApplyFilter(libecap::shared_ptr<MetadataFilter> filter) = 0;

    virtual uint64_t GetLength() const = 0;
};

}

#endif
