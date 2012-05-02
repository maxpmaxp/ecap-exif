#include "MemoryAreaDetails.hpp"

#include <cstdlib>
#include <stdexcept>

#include <libecap/common/errors.h>

using namespace ExifAdapter;

//------------------------------------------------------------------------------
MemoryAreaDetails::MemoryAreaDetails(libecap::size_type size)
    : size(size)
{
    start = static_cast<char*>(std::malloc(size));
    if (start == NULL)
    {
        throw libecap::TextException("Failed to allocate memory");
    }
}

//------------------------------------------------------------------------------
MemoryAreaDetails::~MemoryAreaDetails()
{
    std::free(start);
}

//------------------------------------------------------------------------------
char* MemoryAreaDetails::GetAreaStart()
{
    return start;
}
