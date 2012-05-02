#include "Service.hpp"

#include <iostream>

#include "autoconf.h"

#include "Config.hpp"
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
void Service::configure(const libecap::Options &options)
{
    Log(libecap::flApplication | libecap::ilDebug) << "configure";

    Config* config = Config::GetConfig();
    options.visitEachOption(*config);
}

//------------------------------------------------------------------------------
void Service::reconfigure(const libecap::Options &options)
{
    Log(libecap::flApplication | libecap::ilDebug) << "reconfigure";
    Config* config = Config::GetConfig();
    options.visitEachOption(*config);
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
    (void) url;
    return true;
}

//------------------------------------------------------------------------------
libecap::adapter::Xaction *Service::makeXaction(
    libecap::host::Xaction *hostx)
{
    Log(libecap::flApplication | libecap::ilDebug) << "make xaction";
	return new Xaction(hostx);
}
