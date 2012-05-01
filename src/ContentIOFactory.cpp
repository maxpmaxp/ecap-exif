#include "ContentIOFactory.hpp"

#include "ContentFileIO.hpp"
#include "ContentMemoryIO.hpp"
#include "Log.hpp"

using namespace ExifAdapter;

const char* MULTIPART_FORM_DATA_TYPE = "multipart/form-data";
const uint64_t MEMORY_STORE_LIMIT = 512 * 1024;

//------------------------------------------------------------------------------
static libecap::shared_ptr<ContentIO> GetContentIO(
    const std::string& content_type,
    bool store_in_memory,
    uint64_t length)
{
    libecap::shared_ptr<ContentIO> content_io;

    // if (content_type.find(MULTIPART_FORM_DATA_TYPE) == 0)
    // {
    //     content_io.reset(
    //         new MultipartContentIO(content_type));
    // }
    // else
    if (store_in_memory)
    {
        Log(libecap::flXaction | libecap::ilDebug) << "store in memory";
        content_io.reset(new ContentMemoryIO(length));
    }
    else
    {
        Log(libecap::flXaction | libecap::ilDebug) << "store on disk";
        content_io = ContentFileIO::FromTemporaryFile();
    }

    return content_io;
}

//------------------------------------------------------------------------------
libecap::shared_ptr<ContentIO> ContentIOFactory::CreateContentIO(
    const std::string& content_type)
{
    return GetContentIO(content_type, false, 0);
}

//------------------------------------------------------------------------------
libecap::shared_ptr<ContentIO> ContentIOFactory::CreateContentIO(
    const std::string& content_type,
    uint64_t content_length)
{
    return GetContentIO(
        content_type,
        content_length <= MEMORY_STORE_LIMIT,
        content_length);
}
