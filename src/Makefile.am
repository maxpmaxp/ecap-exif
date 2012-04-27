EXTRA_DIST = \
	Adapter.cpp \
	ContentFileIO.cpp \
	ContentMemoryIO.hpp \
	ExivMetadataFilter.cpp \
	Log.cpp \
	MemoryAreaDetails.cpp \
	Service.cpp \
	Xaction.cpp

lib_LTLIBRARIES = \
	ecap_adapter_exif.la

noinst_HEADERS = \
	ContentIO.hpp \
	ContentFileIO.hpp \
	ContentMemoryIO.hpp \
	ExivMetadataFilter.hpp \
	Log.hpp \
	MemoryAreaDetails.hpp \
	MetadataFilter.hpp \
	Service.hpp \
	Xaction.hpp \
	\
	autoconf.h

ecap_adapter_exif_la_SOURCES = \
	Adapter.cpp \
	ContentFileIO.cpp \
	ContentMemoryIO.cpp \
	ExivMetadataFilter.cpp \
	MemoryAreaDetails.cpp \
	Log.cpp \
	Service.cpp \
	Xaction.cpp
ecap_adapter_exif_la_LDFLAGS = -module -avoid-version $(libecap_LIBS) $(libexiv2_LIBS)

DISTCLEANFILES = \
        autoconf.h

AM_CPPFLAGS = -I$(top_srcdir)/src $(libecap_CFLAGS) $(libecap_CFLAGS)
