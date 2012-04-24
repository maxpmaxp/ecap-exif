#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include <libecap/adapter/service.h>
#include <libecap/adapter/xaction.h>

namespace ExifAdapter
{

class Service:
        public libecap::adapter::Service
{
public:
    // About
    virtual std::string uri() const; // unique across all vendors
    virtual std::string tag() const; // changes with version and config
    virtual void describe(std::ostream &os) const; // free-format info

    // Configuration
    virtual void configure(const libecap::Options &cfg);
    virtual void reconfigure(const libecap::Options &cfg);

    // Lifecycle
    virtual void start(); // expect makeXaction() calls
    virtual void stop(); // no more makeXaction() calls until start()
    virtual void retire(); // no more makeXaction() calls

    // Scope (XXX: this may be changed to look at the whole header)
    virtual bool wantsUrl(const char *url) const;

    // Work
    virtual libecap::adapter::Xaction *makeXaction(
        libecap::host::Xaction *hostx);
};

}

#endif
