#include "Log.hpp"

using namespace ExifAdapter;

#include <libecap/common/registry.h>
#include <libecap/host/host.h>

//------------------------------------------------------------------------------
Log::Log(const libecap::LogVerbosity& verbosity)
    : debug_stream(libecap::MyHost().openDebug(verbosity))
{
    if (debug_stream)
    {
        *debug_stream << "EXIF-eCAP: ";
    }
}

//------------------------------------------------------------------------------
Log::~Log()
{
    if (debug_stream)
    {
        libecap::MyHost().closeDebug(debug_stream);
    }
}
