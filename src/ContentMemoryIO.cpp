#include "ContentMemoryIO.hpp"

#include <cstdlib>
#include <cstring>
#include <stdexcept>

using namespace ExifAdapter;

//------------------------------------------------------------------------------
ContentMemoryIO::ContentMemoryIO(libecap::size_type expected_size)
    : size(expected_size)
    , offset(0)
{
    buffer = static_cast<uint8_t*>(std::malloc(expected_size));
    if (buffer == NULL)
    {
        throw std::runtime_error("Failed to allocate memory");
    }
}

//------------------------------------------------------------------------------
ContentMemoryIO::~ContentMemoryIO()
{
    std::free(buffer);
}

//------------------------------------------------------------------------------
libecap::size_type ContentMemoryIO::Write(const libecap::Area& data)
{
    const libecap::size_type new_size = data.size + offset;
    if (new_size > size)
    {
        uint8_t* buffer = static_cast<uint8_t*>(
            std::realloc(this->buffer, new_size));
        if (buffer == NULL)
        {
            throw std::runtime_error("Failed to allocate memory");
        }
        this->buffer = buffer;
        size = new_size;
    }

    memcpy(buffer + offset, data.start, data.size);
    offset += data.size;

    return data.size;
}

//------------------------------------------------------------------------------
void ContentMemoryIO::Shift(libecap::size_type bytes)
{
    offset += bytes;
}

//------------------------------------------------------------------------------
void ContentMemoryIO::ResetOffset()
{
    offset = 0;
}

//------------------------------------------------------------------------------
libecap::Area ContentMemoryIO::Read(
    libecap::size_type offset,
    libecap::size_type size)
{
    const libecap::size_type position = this->offset + offset;

    if (position > this->size)
    {
        return libecap::Area();
    }

    libecap::size_type correct_size =
        std::min(size, this->size - position);

    if (correct_size == 0)
    {
        return libecap::Area();
    }

    char buffer[correct_size];

    memcpy(buffer, this->buffer + position, correct_size);

    // TODO: copy directly to area
    return libecap::Area::FromTempBuffer(buffer, correct_size);
}

//------------------------------------------------------------------------------
void ContentMemoryIO::ApplyFilter(
    libecap::shared_ptr<MetadataFilter> filter)
{
    int size = this->size; // because of data type
    filter->ProcessMemory(&buffer, &size);
    this->size = size;
}

//------------------------------------------------------------------------------
uint64_t ContentMemoryIO::GetLength() const
{
    return size;
}
