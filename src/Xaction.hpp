#ifndef __XACTION_HPP__
#define __XACTION_HPP__

#include <libecap/common/forward.h>
#include <libecap/adapter/xaction.h>

#include "ContentIO.hpp"

namespace ExifAdapter
{

class Xaction:
        public libecap::adapter::Xaction
{
public:
    Xaction(libecap::host::Xaction *x);
    virtual ~Xaction();

    // meta-info for the host transaction
    virtual const libecap::Area option(
        const libecap::Name &name) const;
    virtual void visitEachOption(
        libecap::NamedValueVisitor &visitor) const;

    // lifecycle
    virtual void start();
    virtual void stop();

    // adapted body transmission control
    virtual void abDiscard();
    virtual void abMake();
    virtual void abMakeMore();
    virtual void abStopMaking();

    // adapted body content extraction and consumption
    virtual libecap::Area abContent(
        libecap::size_type,
        libecap::size_type);
    virtual void abContentShift(libecap::size_type);

    // virgin body state notification
    virtual void noteVbContentDone(bool);
    virtual void noteVbContentAvailable();

    // libecap::Callable API, via libecap::host::Xaction
    virtual bool callable() const;

    bool shouldProcess() const;
    void createAdaptedContentIo();

private:
    libecap::host::Xaction *hostx; // Host transaction rep

    int vb_offset;
    bool vb_at_end;

    libecap::shared_ptr<ContentIO> content;
};

}

#endif
