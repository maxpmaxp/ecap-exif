EXTRA_DIST = \
	Adapter.cpp \
	ContentFileIO.cpp \
	ContentIOFactory.cpp \
	ContentMemoryIO.hpp \
	ExivMetadataFilter.cpp \
	Log.cpp \
	MemoryAreaDetails.cpp \
	MetadataFilterFactory.cpp \
	MultipartContentIO.cpp \
	Service.cpp \
	Xaction.cpp

lib_LTLIBRARIES = \
	ecap_adapter_exif.la

noinst_HEADERS = \
	ContentFileIO.hpp \
	ContentIO.hpp \
	ContentIOFactory.hpp \
	ContentMemoryIO.hpp \
	ExivMetadataFilter.hpp \
	Log.hpp \
	MemoryAreaDetails.hpp \
	MetadataFilter.hpp \
	MetadataFilterFactory.hpp \
	MultipartContentIO.hpp \
	Service.hpp \
	Xaction.hpp \
	\
	autoconf.h

ecap_adapter_exif_la_SOURCES = \
	Adapter.cpp \
	ContentFileIO.cpp \
	ContentIOFactory.cpp \
	ContentMemoryIO.cpp \
	ExivMetadataFilter.cpp \
	MemoryAreaDetails.cpp \
	MetadataFilterFactory.cpp \
	MultipartContentIO.cpp \
	Log.cpp \
	Service.cpp \
	Xaction.cpp
ecap_adapter_exif_la_LDFLAGS = -module -avoid-version $(libecap_LIBS) $(libexiv2_LIBS)

DISTCLEANFILES = \
        autoconf.h

AM_CPPFLAGS = -I$(top_srcdir)/src $(libecap_CFLAGS) $(libecap_CFLAGS)
