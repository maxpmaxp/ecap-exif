EXTRA_DIST = \
	Adapter.cpp \
	Config.cpp \
	ContentFileIO.cpp \
	ContentIOFactory.cpp \
	ContentMemoryIO.hpp \
	ExivMetadataFilter.cpp \
	Log.cpp \
	MemoryAreaDetails.cpp \
	MetadataFilterFactory.cpp \
	Mp3MetadataFilter.cpp \
	Mp4MetadataFilter.cpp \
	MultipartContentIO.cpp \
	OdfMetadataFilter.cpp \
	OggVorbisMetadataFilter.cpp \
	OpenXmlMetadataFilter.cpp \
	PdfMetadataFilter.cpp \
	RuntimeMetadataFilter.cpp \
	Service.cpp \
	Xaction.cpp

lib_LTLIBRARIES = \
	ecap_adapter_exif.la

noinst_HEADERS = \
	Config.hpp \
	ContentFileIO.hpp \
	ContentIO.hpp \
	ContentIOFactory.hpp \
	ContentMemoryIO.hpp \
	ExivMetadataFilter.hpp \
	Log.hpp \
	MemoryAreaDetails.hpp \
	MetadataFilter.hpp \
	MetadataFilterFactory.hpp \
	Mp3MetadataFilter.hpp \
	Mp4MetadataFilter.hpp \
	MultipartContentIO.hpp \
	OdfMetadataFilter.hpp \
	OggVorbisMetadataFilter.hpp \
	OpenXmlMetadataFilter.hpp \
	PdfMetadataFilter.hpp \
	RuntimeMetadataFilter.hpp \
	Service.hpp \
	Xaction.hpp \
	\
	autoconf.h

ecap_adapter_exif_la_SOURCES = \
	Adapter.cpp \
	Config.cpp \
	ContentFileIO.cpp \
	ContentIOFactory.cpp \
	ContentMemoryIO.cpp \
	ExivMetadataFilter.cpp \
	MemoryAreaDetails.cpp \
	MetadataFilterFactory.cpp \
	Mp3MetadataFilter.cpp \
	Mp4MetadataFilter.cpp \
	MultipartContentIO.cpp \
	OdfMetadataFilter.cpp \
	OggVorbisMetadataFilter.cpp \
	OpenXmlMetadataFilter.cpp \
	PdfMetadataFilter.cpp \
	RuntimeMetadataFilter.cpp \
	Log.cpp \
	Service.cpp \
	Xaction.cpp
ecap_adapter_exif_la_LDFLAGS = -module -avoid-version $(libecap_LIBS) $(libexiv2_LIBS) $(taglib_LIBS) $(libzip_LIBS)

DISTCLEANFILES = \
        autoconf.h

AM_CPPFLAGS = -I$(top_srcdir)/src $(libecap_CFLAGS) $(libecap_CFLAGS) $(taglib_CFLAGS) $(libzip_CFLAGS) -Wall -Wextra
