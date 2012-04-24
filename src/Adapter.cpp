#include "Service.hpp"

#include <libecap/common/registry.h>

// create the adapter and register with libecap to reach the host application
static const bool Registered = (libecap::RegisterService(new ExifAdapter::Service), true);
