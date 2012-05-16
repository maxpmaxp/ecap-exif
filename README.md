# eCAP EXIF metadata filter

eCAP adapter for stripping EXIF and other metadata from files,
primarly JPEG.

## Installation

libecap and exiv2 libraries are required to build and use the
adapter. libecap is available at http://e-cap.org/. The adapter can be
built and installed from sources, usually by running:

    $ ./bootstrap.sh
    $ ./configure
    $ make
    $ make install

## Configuration

* `tmp_filename_format`
    Set the format of temporary files that will be processed
    by the adapter.
* `memory_filesize_limit`
    Files with size greater than limit will be stored in temporary
    disk storage, otherwise processing will be done in RAM.

## Supported MIME types

* image/jpeg
* image/x-exv
* image/x-canon-cr2
* image/x-canon-crw
* image/tiff
* image/x-nikon-nef
* image/x-pentax-pef
* image/x-samsung-srw
* image/x-olympus-orf
* image/png
* image/pgf
* application/postscript
* application/rdf+xml
* image/x-photoshop
* image/jp2
* audio/3gpp
* audio/3gpp2
* video/3gpp
* video/3gpp2
* audio/mp4
* audio/mp4a-latm
* audio/x-m4a
* video/mp4
* video/quicktime
* audio/mpeg
* audio/ogg
* application/pdf
* application/vnd.oasis.opendocument.text
* application/vnd.oasis.opendocument.presentation
* application/vnd.oasis.opendocument.spreadsheet
* application/vnd.oasis.opendocument.graphics
* application/vnd.openxmlformats-officedocument.wordprocessingml.document
* application/vnd.openxmlformats-officedocument.spreadsheetml.sheet
* application/vnd.openxmlformats-officedocument.presentationml.presentation

Adapter is capable of processing messages with MIME type multipart/form-data by parsing the message and applying corresponding filter for every part of the message.

Messages with MIME type application/octet-stream are handled in a special way by checking actual type of the message content and applying corresonding filter (if any).

## Squid configuration

Example configuration for stripping metadata from outgoing requests.

    ecap_enable on
    loadable_modules /usr/local/lib/ecap_adapter_exif.so
    ecap_service eReqmod reqmod_precache 0 ecap://sterch.net/ecap/services/exif-filter

    adaptation_service_set reqFilter eReqmod
    adaptation_access reqFilter allow all

To log debug messages use:

    debug_options ALL,1 93,9
