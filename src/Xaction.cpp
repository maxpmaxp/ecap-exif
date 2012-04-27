#include "Xaction.hpp"

#include <sstream>
#include <stdexcept>

#include <libecap/common/errors.h>
#include <libecap/common/header.h>
#include <libecap/common/message.h>
#include <libecap/common/name.h>
#include <libecap/common/names.h>
#include <libecap/host/xaction.h>

#include "ContentFileIO.hpp"
#include "ContentMemoryIO.hpp"
#include "ExivMetadataFilter.hpp"
#include "Log.hpp"

using namespace ExifAdapter;

const libecap::size_type MAX_AB_CONTENT_SIZE = 32 * 1024;
const uint64_t MEMORY_STORE_LIMIT = 512 * 1024;

//------------------------------------------------------------------------------
Xaction::Xaction(libecap::host::Xaction *x)
    : hostx(x)
    , vb_offset(0)
    , vb_at_end(false)
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

	Must(hostx);

    if (!shouldProcess())
    {
        // make this adapter non-callable
        libecap::host::Xaction *x = hostx;
        hostx = 0;

        // tell the host to use the virgin message
        x->useVirgin();
        return;
    }

    createAdaptedContentIo();

    hostx->vbMake();
}

//------------------------------------------------------------------------------
void Xaction::stop()
{
    Log(libecap::flXaction | libecap::ilDebug) << "stop";

	hostx = 0;
	// the caller will delete

    content.reset();
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

    vb_at_end = at_end;

    Must(hostx);
    hostx->vbStopMaking();

    if (vb_offset == 0)
    {
        // there were no body
        hostx->useVirgin();
        return;
    }

    libecap::shared_ptr<MetadataFilter> filter(new ExivMetadataFilter());
    try
    {
        content->ApplyFilter(filter);
    }
    catch (std::exception& e)
    {
        Log(libecap::flXaction | libecap::ilDebug)
            << "metadata filter failed to process data "
            << ": " << e.what();
        hostx->useVirgin();
        return;
    }

    libecap::shared_ptr<libecap::Message> adapted = hostx->virgin().clone();
    Must(adapted != 0);

    adapted->header().removeAny(libecap::headerContentLength);

    try
    {
        uint64_t content_length = content->GetLength();

        std::stringstream output;
        output << content_length;

        const libecap::Header::Value length =
            libecap::Area::FromTempString(output.str());
        adapted->header().add(libecap::headerContentLength, length);
    }
    catch (std::runtime_error& e)
    {
        Log(libecap::flXaction | libecap::ilDebug)
            << "failed to get content length: "
            << e.what();
    }

    hostx->useAdapted(adapted);
}

//------------------------------------------------------------------------------
void Xaction::noteVbContentAvailable()
{
    Log(libecap::flXaction | libecap::ilDebug) << "noteVbContentAvailable";

    Must(hostx);

    const libecap::Area vb = hostx->vbContent(0, libecap::nsize);

    try
    {
        const size_t written = content->Write(vb);
        vb_offset += written;
        hostx->vbContentShift(written);
    }
    catch (std::runtime_error& e)
    {
        Log(libecap::flXaction | libecap::ilDebug)
            << "failed to write vb data: "
            << e.what();
    }
}

//------------------------------------------------------------------------------
bool Xaction::callable() const
{
    return hostx != 0; // no point to call us if we are done
}

//------------------------------------------------------------------------------
bool Xaction::shouldProcess() const
{
    if (!hostx->virgin().body())
    {
        Log(libecap::flXaction | libecap::ilDebug) << "no body";
        return false;
    }

    const libecap::Name content_type_header("Content-Type");
    const char* type_image_jpeg = "image/jpeg";

    const libecap::Header &header = hostx->virgin().header();
    if (!header.hasAny(content_type_header))
    {
        Log(libecap::flXaction | libecap::ilDebug) << "no content type";
        return false;
    }

    const libecap::Area type = header.value(content_type_header);
    const std::string content_type_value(type.start, type.size);
    if (content_type_value != type_image_jpeg)
    {
        Log(libecap::flXaction | libecap::ilDebug)
            << "content type " << content_type_value
            << " is not supported";
        return false;
    }

    Log(libecap::flXaction | libecap::ilDebug) << "should be processed";

    return true;
}

//------------------------------------------------------------------------------
void Xaction::createAdaptedContentIo()
{
    Must(!content);
    Must(hostx);

    bool store_in_memory = false;
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

        if (length <= MEMORY_STORE_LIMIT)
        {
            store_in_memory = true;
        }
    }

    if (store_in_memory)
    {
        Log(libecap::flXaction | libecap::ilDebug) << "store in memory";
        content.reset(new ContentMemoryIO(length));
    }
    else
    {
        Log(libecap::flXaction | libecap::ilDebug) << "store on disk";
        content = ContentFileIO::FromTemporaryFile();
    }

    Must(content);
}
