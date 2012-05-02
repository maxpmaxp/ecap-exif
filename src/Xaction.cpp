#include "Xaction.hpp"

#ifdef MEASURE_EXECUTION_TIME
#include <cstring>
#endif

#include <sstream>
#include <stdexcept>

#include <libecap/common/errors.h>
#include <libecap/common/header.h>
#include <libecap/common/message.h>
#include <libecap/common/name.h>
#include <libecap/common/names.h>
#include <libecap/host/xaction.h>

#include "ContentIOFactory.hpp"
#include "Log.hpp"
#include "MetadataFilterFactory.hpp"

#ifdef MEASURE_EXECUTION_TIME
static uint64_t GetCurrentTime()
{
    struct timespec tp;
    memset(&tp, 0, sizeof(tp));
    clock_gettime(CLOCK_MONOTONIC, &tp);
    uint64_t time = (uint64_t)tp.tv_sec * (uint64_t)1000000000 + (uint64_t)tp.tv_nsec;
    return time;
}
#endif

using namespace ExifAdapter;

const libecap::size_type MAX_AB_CONTENT_SIZE = 32 * 1024;

//------------------------------------------------------------------------------
Xaction::Xaction(libecap::host::Xaction *x)
    : hostx(x)
    , vb_offset(0)
    , vb_at_end(false)
#ifdef MEASURE_EXECUTION_TIME
    , start_time(0)
    , first_data_received_time(0)
    , all_data_received_time(0)
    , data_processed_time(0)
#endif
{
    Log(libecap::flXaction | libecap::ilDebug) << "Xaction";
}

//------------------------------------------------------------------------------
Xaction::~Xaction()
{
    Log(libecap::flXaction | libecap::ilDebug) << "~Xaction";

	if (libecap::host::Xaction *x = hostx)
    {
		hostx = 0;
		x->adaptationAborted();
	}
}

//------------------------------------------------------------------------------
const libecap::Area Xaction::option(const libecap::Name &) const
{
    Log(libecap::flXaction | libecap::ilDebug) << "option";

	return libecap::Area(); // this transaction has no meta-information
}

//------------------------------------------------------------------------------
void Xaction::visitEachOption(libecap::NamedValueVisitor &) const {
	// this transaction has no meta-information to pass to the visitor
    Log(libecap::flXaction | libecap::ilDebug) << "visitEachOption";
}

//------------------------------------------------------------------------------
void Xaction::start()
{
    Log(libecap::flXaction | libecap::ilDebug) << "start";

#ifdef MEASURE_EXECUTION_TIME
    start_time = GetCurrentTime();
#endif

	Must(hostx);

    const std::string content_type = getContentType();

    filter = MetadataFilterFactory::CreateFilter(content_type);

    if (!shouldProcess(content_type))
    {
        // make this adapter non-callable
        libecap::host::Xaction *x = hostx;
        hostx = 0;

        // tell the host to use the virgin message
        x->useVirgin();
        return;
    }

    createAdaptedContentIo(content_type);

    hostx->vbMake();
}

//------------------------------------------------------------------------------
void Xaction::stop()
{
    Log(libecap::flXaction | libecap::ilDebug) << "stop";

	hostx = 0;
	// the caller will delete

    content.reset();
    filter.reset();

#ifdef MEASURE_EXECUTION_TIME
    uint64_t stop_time = GetCurrentTime();
    uint64_t all_time = stop_time - start_time;

    Log(libecap::flXaction | libecap::ilDebug)
        << "EXIF-eCAP: Xaction execution time: " << all_time
        << " first data: " << first_data_received_time - start_time
        << " all_data: " << all_data_received_time - first_data_received_time
        << " data_processed_time: " << data_processed_time - all_data_received_time
        << " send_data: " << stop_time - data_processed_time;
#endif
}

//------------------------------------------------------------------------------
void Xaction::abDiscard()
{
    Log(libecap::flXaction | libecap::ilDebug) << "abDiscard";
}

//------------------------------------------------------------------------------
void Xaction::abMake()
{
    Log(libecap::flXaction | libecap::ilDebug) << "abMake";

    content->ResetOffset();

    hostx->noteAbContentAvailable();
    hostx->noteAbContentDone(vb_at_end);
}

//------------------------------------------------------------------------------
void Xaction::abMakeMore()
{
    Log(libecap::flXaction | libecap::ilDebug) << "abMakeMore";
    // FIXME: handle
}

//------------------------------------------------------------------------------
void Xaction::abStopMaking()
{
    Log(libecap::flXaction | libecap::ilDebug) << "abStopMaking";
    // FIXME: handle
}

//------------------------------------------------------------------------------
libecap::Area Xaction::abContent(
    libecap::size_type offset,
    libecap::size_type size)
{
    Log(libecap::flXaction | libecap::ilDebug)
        << "abContent offset: " << offset
        << " size: " << size;

    const libecap::size_type corrected_size =
        std::min(size, MAX_AB_CONTENT_SIZE);

    return content->Read(offset, corrected_size);
}

//------------------------------------------------------------------------------
void Xaction::abContentShift(libecap::size_type bytes)
{
    Log(libecap::flXaction | libecap::ilDebug)
        << "abContentShift bytes: " << bytes;

    content->Shift(bytes);
}

//------------------------------------------------------------------------------
void Xaction::noteVbContentDone(bool at_end)
{
    Log(libecap::flXaction | libecap::ilDebug)
        << "noteVbContentDone at_end: " << at_end;

#ifdef MEASURE_EXECUTION_TIME
    all_data_received_time = GetCurrentTime();
#endif

    vb_at_end = at_end;

    Must(hostx);
    hostx->vbStopMaking();

    if (vb_offset == 0)
    {
        // there were no body
        hostx->useVirgin();
        return;
    }

    try
    {
        content->ApplyFilter(filter);
    }
    catch (MetadataFilter::Exception& e)
    {
        Log(libecap::flXaction | libecap::ilDebug)
            << "metadata filter failed to process data "
            << ": " << e.what();
        // host probably already got rid of virgin body
        // so we have to return him unprocessed one
    }

#ifdef MEASURE_EXECUTION_TIME
    data_processed_time = GetCurrentTime();
#endif

    libecap::shared_ptr<libecap::Message> adapted = hostx->virgin().clone();
    Must(adapted != 0);

    adapted->header().removeAny(libecap::headerContentLength);

    uint64_t content_length = content->GetLength();

    std::stringstream output;
    output << content_length;

    const libecap::Header::Value length =
        libecap::Area::FromTempString(output.str());
    adapted->header().add(libecap::headerContentLength, length);

    hostx->useAdapted(adapted);
}

//------------------------------------------------------------------------------
void Xaction::noteVbContentAvailable()
{
    Log(libecap::flXaction | libecap::ilDebug) << "noteVbContentAvailable";

#ifdef MEASURE_EXECUTION_TIME
    if (first_data_received_time == 0)
    {
        first_data_received_time = GetCurrentTime();
    }
#endif

    Must(hostx);

    const libecap::Area vb = hostx->vbContent(0, libecap::nsize);

    const size_t written = content->Write(vb);
    vb_offset += written;
    hostx->vbContentShift(written);
}

//------------------------------------------------------------------------------
bool Xaction::callable() const
{
    return hostx != 0; // no point to call us if we are done
}

//------------------------------------------------------------------------------
bool Xaction::shouldProcess(const std::string& content_type) const
{
    if (!hostx->virgin().body())
    {
        Log(libecap::flXaction | libecap::ilDebug) << "no body";
        return false;
    }

    if (content_type.empty())
    {
        return false;
    }

    if (!MetadataFilterFactory::IsMimeTypeSupported(content_type))
    {
        Log(libecap::flXaction | libecap::ilDebug)
            << "content type " << content_type
            << " is not supported";
        return false;
    }

    Log(libecap::flXaction | libecap::ilDebug)
        << "content type "
        << content_type
        << " should be processed";

    return true;
}

//------------------------------------------------------------------------------
void Xaction::createAdaptedContentIo(const std::string& content_type)
{
    Must(!content);
    Must(hostx);

    bool length_found = false;
    uint64_t length = 0;

    const libecap::Header &header = hostx->virgin().header();
    if (!header.hasAny(libecap::headerContentLength))
    {
        Log(libecap::flXaction | libecap::ilDebug) << "no content length";
    }
    else
    {
        const libecap::Area area =
            header.value(libecap::headerContentLength);
        const std::string length_value(area.start, area.size);
        std::istringstream is(length_value);
        if (!(is >> length))
        {
            Log(libecap::flXaction | libecap::ilDebug) << "malformed content length";
        }

        length_found = true;
    }

    if (length_found)
    {
        content = ContentIOFactory::CreateContentIO(content_type, length);
    }
    else
    {
        content = ContentIOFactory::CreateContentIO(content_type);
    }

    Must(content);
}

//------------------------------------------------------------------------------
std::string Xaction::getContentType() const
{
    const libecap::Name content_type_header("Content-Type");

    const libecap::Header &header = hostx->virgin().header();
    if (!header.hasAny(content_type_header))
    {
        Log(libecap::flXaction | libecap::ilDebug) << "no content type";
        return "";
    }

    const libecap::Area type = header.value(content_type_header);
    const std::string content_type_value(type.start, type.size);
    return content_type_value;
}
