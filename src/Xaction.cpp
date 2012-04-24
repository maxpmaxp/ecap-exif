#include "Xaction.hpp"

#include <libecap/common/errors.h>
#include <libecap/host/xaction.h>

#include "Log.hpp"

using namespace ExifAdapter;

//------------------------------------------------------------------------------
Xaction::Xaction(libecap::host::Xaction *x)
    : hostx(x)
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

	// make this adapter non-callable
	libecap::host::Xaction *x = hostx;
	hostx = 0;

	// tell the host to use the virgin message
	x->useVirgin();
}

//------------------------------------------------------------------------------
void Xaction::stop()
{
    Log(libecap::flXaction | libecap::ilDebug) << "stop";

	hostx = 0;
	// the caller will delete
}

//------------------------------------------------------------------------------
void Xaction::abDiscard()
{
    Log(libecap::flXaction | libecap::ilDebug) << "abDiscard";

    noBodySupport();
}

//------------------------------------------------------------------------------
void Xaction::abMake()
{
    Log(libecap::flXaction | libecap::ilDebug) << "abMake";

    noBodySupport();
}

//------------------------------------------------------------------------------
void Xaction::abMakeMore()
{
    Log(libecap::flXaction | libecap::ilDebug) << "abMakeMore";

    noBodySupport();
}

//------------------------------------------------------------------------------
void Xaction::abStopMaking()
{
    Log(libecap::flXaction | libecap::ilDebug) << "abStopMaking";

    noBodySupport();
}

//------------------------------------------------------------------------------
libecap::Area Xaction::abContent(
    libecap::size_type offset,
    libecap::size_type size)
{
    Log(libecap::flXaction | libecap::ilDebug)
        << "abContent offset: " << offset
        << " size: " << size;

    noBodySupport();
    return libecap::Area();
}

//------------------------------------------------------------------------------
void Xaction::abContentShift(libecap::size_type bytes)
{
    Log(libecap::flXaction | libecap::ilDebug)
        << "abContentShift bytes: " << bytes;

    noBodySupport();
}

//------------------------------------------------------------------------------
void Xaction::noteVbContentDone(bool at_end)
{
    Log(libecap::flXaction | libecap::ilDebug)
        << "noteVbContentDone at_end: " << at_end;

    noBodySupport();
}

//------------------------------------------------------------------------------
void Xaction::noteVbContentAvailable()
{
    Log(libecap::flXaction | libecap::ilDebug) << "noteVbContentAvailable";

    noBodySupport();
}

//------------------------------------------------------------------------------
bool Xaction::callable() const
{
    return hostx != 0; // no point to call us if we are done
}

//------------------------------------------------------------------------------
void Xaction::noBodySupport() const
{
	Must(!"must not be called: minimal adapter offers no body support");
	// not reached
}
