EXTRA_DIST = \
	Adapter.cpp \
	Service.cpp \
	Xaction.cpp

lib_LTLIBRARIES = \
	ecap_adapter_exif.la

noinst_HEADERS = \
	Service.hpp \
	Xaction.hpp \
	\
	autoconf.h

ecap_adapter_exif_la_SOURCES = Adapter.cpp Service.cpp Xaction.cpp
ecap_adapter_exif_la_LDFLAGS = -module -avoid-version $(libecap_LIBS)

# -shared -export-dynamic -Wl,-soname,ecap_noop_adapter.so

DISTCLEANFILES = \
        autoconf.h

AM_CPPFLAGS = -I$(top_srcdir)/src $(libecap_CFLAGS)
