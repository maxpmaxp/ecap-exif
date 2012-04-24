#include "Xaction.hpp"

#include <libecap/common/errors.h>

using namespace ExifAdapter;

//------------------------------------------------------------------------------
Xaction::Xaction(libecap::host::Xaction *x)
    : hostx(x)
{
}

//------------------------------------------------------------------------------
Xaction::~Xaction()
{
	if (libecap::host::Xaction *x = hostx)
    {
		hostx = 0;
		x->adaptationAborted();
	}
}

//------------------------------------------------------------------------------
const libecap::Area Xaction::option(const libecap::Name &) const
{
	return libecap::Area(); // this transaction has no meta-information
}

//------------------------------------------------------------------------------
void Xaction::visitEachOption(libecap::NamedValueVisitor &) const {
	// this transaction has no meta-information to pass to the visitor
}

//------------------------------------------------------------------------------
void Xaction::start()
{
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
	hostx = 0;
	// the caller will delete
}

//------------------------------------------------------------------------------
bool Xaction::callable() const {
	return hostx != 0; // no point to call us if we are done
}

//------------------------------------------------------------------------------
void Xaction::noBodySupport() const {
	Must(!"must not be called: minimal adapter offers no body support");
	// not reached
}
