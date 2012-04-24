#include "Service.hpp"

#include <iostream>

#include "autoconf.h"

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
}

//------------------------------------------------------------------------------
void Service::reconfigure(const libecap::Options &)
{
	// this service is not configurable
}

//------------------------------------------------------------------------------
void Service::start()
{
	libecap::adapter::Service::start();
	// custom code would go here, but this service does not have one
}

//------------------------------------------------------------------------------
void Service::stop()
{
	// custom code would go here, but this service does not have one
	libecap::adapter::Service::stop();
}

//------------------------------------------------------------------------------
void Service::retire()
{
	// custom code would go here, but this service does not have one
	libecap::adapter::Service::stop();
}

//------------------------------------------------------------------------------
bool Service::wantsUrl(const char *url) const
{
	return true; // minimal adapter is applied to all messages
}

//------------------------------------------------------------------------------
libecap::adapter::Xaction *Service::makeXaction(
    libecap::host::Xaction *hostx)
{
	return new Xaction(hostx);
}
