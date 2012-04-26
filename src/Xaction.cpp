#include "Xaction.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <sstream>
#include <sys/stat.h>

#include <libecap/common/errors.h>
#include <libecap/common/header.h>
#include <libecap/common/message.h>
#include <libecap/common/name.h>
#include <libecap/common/names.h>
#include <libecap/host/xaction.h>

#include "ExivMetadataFilter.hpp"
#include "Log.hpp"

using namespace ExifAdapter;

const char* TEMPORARY_FILENAME_FORMAT = "/tmp/exif-ecap-XXXXXX";
const libecap::size_type MAX_AB_CONTENT_SIZE = 32 * 1024;

//------------------------------------------------------------------------------
Xaction::Xaction(libecap::host::Xaction *x)
    : hostx(x)
    , tmp_fd(0)
    , vb_offset(0)
    , ab_offset(0)
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

    hostx->vbMake();
}

//------------------------------------------------------------------------------
void Xaction::stop()
{
    Log(libecap::flXaction | libecap::ilDebug) << "stop";

	hostx = 0;
	// the caller will delete

    removeTemporaryFile();
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

    ab_offset = 0;
    hostx->noteAbContentAvailable();
    hostx->noteAbContentDone(vb_at_end);
}

//------------------------------------------------------------------------------
void Xaction::abMakeMore()
{
    Log(libecap::flXaction | libecap::ilDebug) << "abMakeMore";
}

//------------------------------------------------------------------------------
void Xaction::abStopMaking()
{
    Log(libecap::flXaction | libecap::ilDebug) << "abStopMaking";
}

//------------------------------------------------------------------------------
libecap::Area Xaction::abContent(
    libecap::size_type offset,
    libecap::size_type size)
{
    Log(libecap::flXaction | libecap::ilDebug)
        << "abContent offset: " << offset
        << " size: " << size;

    if (size == 0)
    {
        return libecap::Area();
    }

    Must(tmp_fd);

    const libecap::size_type position = ab_offset + offset;
    Must(lseek(tmp_fd, position, SEEK_SET) != -1);

    const libecap::size_type buffer_size =
        std::min(size, MAX_AB_CONTENT_SIZE);
    char buffer[buffer_size];

    const ssize_t result = read(tmp_fd, buffer, sizeof(buffer));
    if (result != -1)
    {
        return libecap::Area::FromTempBuffer(buffer, result);
    }

    return libecap::Area();
}

//------------------------------------------------------------------------------
void Xaction::abContentShift(libecap::size_type bytes)
{
    Log(libecap::flXaction | libecap::ilDebug)
        << "abContentShift bytes: " << bytes;

    ab_offset += bytes;
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

    closeTemporaryFile();

    ExivMetadataFilter filter;
    try
    {
        filter.ProcessFile(tmp_filename);
    }
    catch (std::exception& e)
    {
        Log(libecap::flXaction | libecap::ilDebug)
            << "metadata filter failed to process file "
            << tmp_filename
            << ": " << e.what();
        hostx->useVirgin();
        return;
    }

    tmp_fd = open(tmp_filename.c_str(), O_RDONLY);
    if (tmp_fd == -1)
    {
        tmp_fd = 0;
        Log(libecap::flXaction | libecap::ilDebug)
            << "failed to open processed file";
        hostx->useVirgin();
        return;
    }

    libecap::shared_ptr<libecap::Message> adapted = hostx->virgin().clone();
    Must(adapted != 0);

    adapted->header().removeAny(libecap::headerContentLength);

    struct stat statbuf;

    if (fstat(tmp_fd, &statbuf) == -1)
    {
        Log(libecap::flXaction | libecap::ilDebug)
            << "failed to get temporary file size: "
            << strerror(errno);
    }
    else
    {
        std::stringstream output;
        output << statbuf.st_size;

        const libecap::Header::Value length =
            libecap::Area::FromTempString(output.str());
        adapted->header().add(libecap::headerContentLength, length);
    }

    hostx->useAdapted(adapted);
}

//------------------------------------------------------------------------------
void Xaction::noteVbContentAvailable()
{
    Log(libecap::flXaction | libecap::ilDebug) << "noteVbContentAvailable";

    Must(hostx);

    const libecap::Area vb = hostx->vbContent(0, libecap::nsize);

    if (!tmp_fd && !openTemporaryFile())
    {
        return;
    }

    const size_t written = write(tmp_fd, vb.start, vb.size);
    if (written != vb.size)
    {
        Log(libecap::flXaction | libecap::ilDebug)
            << "failed to write data to temporary file: "
            << strerror(errno);
        return;
    }

    vb_offset += written;
    hostx->vbContentShift(written);
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
bool Xaction::openTemporaryFile()
{
    Must(!tmp_fd);

    const int filename_length = 32;
    char temporary_filename[filename_length];
    memset(temporary_filename, 0, filename_length);
    strncpy(temporary_filename, TEMPORARY_FILENAME_FORMAT, filename_length);

    int result = mkstemp(temporary_filename);
    if (result == -1)
    {
        Log(libecap::flXaction | libecap::ilDebug)
            << "failed to create temporary file with format "
            << temporary_filename
            << ": " << strerror(errno);
        return false;
    }

    tmp_filename = temporary_filename;

    tmp_fd = result;
    return true;
}

//------------------------------------------------------------------------------
void Xaction::closeTemporaryFile()
{
    if (tmp_fd)
    {
        if (close(tmp_fd) != 0)
        {
            Log(libecap::flXaction | libecap::ilDebug)
                << "failed to close temporary file: "
                << strerror(errno);
        }

        tmp_fd = 0;
    }
}

//------------------------------------------------------------------------------
void Xaction::removeTemporaryFile()
{
    if (!tmp_filename.empty())
    {
        if (remove(tmp_filename.c_str()) != 0)
        {
            Log(libecap::flXaction | libecap::ilDebug)
                << "failed to remove temporary file: "
                << strerror(errno);
        }

        tmp_filename.clear();
    }
}
