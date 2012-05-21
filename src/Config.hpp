#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <stdint.h>
#include <string>
#include <vector>

#include <libecap/common/named_values.h>

namespace ExifAdapter
{

class Config
    : public libecap::NamedValueVisitor
{
public:
    static Config* GetConfig();

    void visit(
        const libecap::Name& name,
        const libecap::Area& value);

    std::string GetTemporaryFilenameFormat();
    uint64_t GetMemoryFilesizeLimit();
    const std::vector<std::string>& GetExcludedTypes() const;

private:
    Config();

    void SetTemporaryFilenameFormat(const std::string& value);
    void SetMemoryFilesizeLimit(const std::string& value);

    std::string tmp_filename_format;
    uint64_t memory_filesize_limit;
    std::vector<std::string> excluded_types;;
};

}

#endif
