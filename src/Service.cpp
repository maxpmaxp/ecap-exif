#include "Service.hpp"

#include <iostream>

#include "autoconf.h"

#include "Log.hpp"
#include "Xaction.hpp"

using namespace ExifAdapter;

//------------------------------------------------------------------------------
std::string Service::uri() const
{
	return "ecap://sterch.net/ecap/services/exif-filter";
}

//------------------------------------------------------------------------------
std::string Service::tag() const
{
	return PACKAGE_VERSION;
}

//------------------------------------------------------------------------------
void Service::describe(std::ostream &os) const
{
	os << "" << PACKAGE_NAME << " v" << PACKAGE_VERSION;
}

//------------------------------------------------------------------------------
void Service::configure(const libecap::Options &)
{
	// this service is not configurable
    Log(libecap::flApplication | libecap::ilDebug) << "configure";
}

//------------------------------------------------------------------------------
void Service::reconfigure(const libecap::Options &)
{
	// this service is not configurable
    Log(libecap::flApplication | libecap::ilDebug) << "reconfigure";
}

//------------------------------------------------------------------------------
void Service::start()
{
    Log(libecap::flApplication | libecap::ilDebug) << "start";
	libecap::adapter::Service::start();
	// custom code would go here, but this service does not have one
}

//------------------------------------------------------------------------------
void Service::stop()
{
    Log(libecap::flApplication | libecap::ilDebug) << "stop";
	// custom code would go here, but this service does not have one
	libecap::adapter::Service::stop();
}

//------------------------------------------------------------------------------
void Service::retire()
{
    Log(libecap::flApplication | libecap::ilDebug) << "retire";
	// custom code would go here, but this service does not have one
	libecap::adapter::Service::stop();
}

//------------------------------------------------------------------------------
bool Service::wantsUrl(const char *url) const
{
	return true;
}

//------------------------------------------------------------------------------
libecap::adapter::Xaction *Service::makeXaction(
    libecap::host::Xaction *hostx)
{
    Log(libecap::flApplication | libecap::ilDebug) << "make xaction";
	return new Xaction(hostx);
}
