#ifndef __MEMORY_AREA_DETAILS_HPP
#define __MEMORY_AREA_DETAILS_HPP

#include <libecap/common/area.h>

namespace ExifAdapter
{

class MemoryAreaDetails
    : public libecap::AreaDetails
{
public:
    MemoryAreaDetails(libecap::size_type size);
    ~MemoryAreaDetails();

    char* GetAreaStart();

private:
    char* start;
    libecap::size_type size;
};

}

#endif
