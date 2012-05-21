#ifndef __METADATA_FILTER_HPP__
#define __METADATA_FILTER_HPP__

#include <stdexcept>
#include <stdint.h>
#include <string>
#include <vector>

namespace ExifAdapter
{

class MetadataFilter
{
public:
    class Exception
        : public std::exception
    {
    public:
        Exception(const std::string& msg)
            : msg(msg)
            {
            }
        virtual ~Exception() throw() {};

        const char* what() const throw()
            {
                return msg.c_str();
            }
    private:
        std::string msg;
    };

    virtual ~MetadataFilter() {};

    // throws Exception
    virtual void ProcessFile(const std::string& path) = 0;

    // can realloc buffer
    // throws Exception
    virtual void ProcessMemory(
        uint8_t** buffer,
        int* size) = 0;

    virtual bool IsMimeTypeSupported(const std::string& mime_type) const;

    // returns number of enabled types
    unsigned DisableMimeTypes(const std::vector<std::string>& mime_types);

    virtual bool CanProcess(const std::string& path) const = 0;
    virtual bool CanProcess(
        uint8_t* buffer,
        int size) const = 0;

    virtual bool SupportsInMemoryProcessing() const = 0;

protected:
    std::vector<std::string> mime_types;
};

}

#endif
