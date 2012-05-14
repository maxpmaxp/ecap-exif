#ifndef __CONTENT_IO_FACTORY_HPP__
#define __CONTENT_IO_FACTORY_HPP__

#include <stdint.h>

#include <libecap/common/memory.h>

#include "ContentIO.hpp"

namespace ExifAdapter
{

class ContentIOFactory
{
public:
    static libecap::shared_ptr<ContentIO> CreateContentIO(
        const std::string& content_type);
    static libecap::shared_ptr<ContentIO> CreateContentIO(
        const std::string& content_type,
        uint64_t content_length,
        bool can_be_stored_in_memory);
};

}

#endif
