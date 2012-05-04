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

## Squid configuration

Example configuration for stripping metadata from outgoing requests.

    ecap_enable on
    loadable_modules /usr/local/lib/ecap_adapter_exif.so
    ecap_service eReqmod reqmod_precache 0 ecap://sterch.net/ecap/services/exif-filter

    adaptation_service_set reqFilter eReqmod
    adaptation_access reqFilter allow all

To log debug messages use:

    debug_options ALL,1 93,9
