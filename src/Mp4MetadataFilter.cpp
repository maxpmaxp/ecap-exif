#include "Mp4MetadataFilter.hpp"

#include <fstream>

#include <errno.h>
#include <fcntl.h>
#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>

#include "Log.hpp"

static const char* mime_types[] = {
    "audio/3gpp",
    "audio/3gpp2",
    "video/3gpp",
    "video/3gpp2",
    "audio/mp4",
    "audio/mp4a-latm",
    "audio/x-m4a",
    "video/mp4",
    "video/quicktime",
    NULL,
};

#define BE_32(x) ((((uint8_t*)(x))[0] << 24) | \
                  (((uint8_t*)(x))[1] << 16) | \
                  (((uint8_t*)(x))[2] << 8) |  \
                  ((uint8_t*)(x))[3])

using namespace ExifAdapter;

//------------------------------------------------------------------------------
Mp4MetadataFilter::Mp4MetadataFilter()
{
}

//------------------------------------------------------------------------------
Mp4MetadataFilter::~Mp4MetadataFilter()
{
}

//------------------------------------------------------------------------------
void Mp4MetadataFilter::ProcessFile(const std::string& path)
{
    int fd = open(path.c_str(), O_RDWR);
    if (fd == -1)
    {
        std::string msg = std::string("Failed to open file ") + path;
        throw MetadataFilter::Exception(msg);
    }

    const int64_t ftyp_size = 12;
    char ftyp[ftyp_size] = {0};

    if (pread(fd, ftyp, ftyp_size, 0) != ftyp_size)
    {
        close(fd);
        throw MetadataFilter::Exception("Failed to read header");
    }

    if (!IsFtypSupported(&ftyp[4]))
    {
        close(fd);
        throw MetadataFilter::Exception("File format is not supported");
    }

    struct stat statbuf;

    if (fstat(fd, &statbuf) == -1)
    {
        std::string msg = std::string("Failed to get file size of ") +
            path + ": " + strerror(errno);
        throw MetadataFilter::Exception(msg);
    }

    int64_t file_size = statbuf.st_size;

    off_t position = lseek(fd, 0, SEEK_CUR);

    if (position == (off_t)-1)
    {
        close(fd);
        throw MetadataFilter::Exception("Failed to get file position");
    }

    const int64_t atom_header_size = 8;
    char atom[atom_header_size] = {0};

    while (position < file_size - 8)
    {
        if (read(fd, atom, atom_header_size) == -1)
        {
            close(fd);
            throw MetadataFilter::Exception("Failed to read atom");
        }

        uint32_t atom_size = (uint32_t)BE_32(&atom[0]);
        std::string atom_type = std::string("") +
            atom[4] + atom[5] + atom[6] + atom[7];

        if (atom_size + position > file_size ||
            atom_size == 0)
        {
            throw MetadataFilter::Exception("Failed to parse");
        }

        if (atom_type == "moov")
        {
            off_t offset = position;
            off_t pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);

            if (offset + atom_size > file_size)
            {
                close(fd);
                throw MetadataFilter::Exception("moov size > file size");
            }

            if (lseek(fd, 0, SEEK_SET) == (off_t)-1)
            {
                close (fd);
                throw MetadataFilter::Exception("Failed to seek to the beginning of file");
            }

            void* mmaped = mmap(
                NULL,
                atom_size + offset - pa_offset,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                fd,
                pa_offset);

            uint8_t *moov = reinterpret_cast<uint8_t*>(mmaped);
            moov = moov + offset - pa_offset;
            try
            {
                ProcessMoov(moov, atom_size);
            }
            catch (MetadataFilter::Exception& e)
            {
                munmap(mmaped, atom_size + offset - pa_offset);
                close(fd);
                throw e;
            }
            munmap(mmaped, atom_size + offset - pa_offset);
            break;
        }

        position = lseek(fd, atom_size - atom_header_size, SEEK_CUR);
        if (position == (off_t)-1)
        {
            close(fd);
            throw MetadataFilter::Exception("Faield to seek");
        }
    }

    close(fd);
}

//------------------------------------------------------------------------------
void Mp4MetadataFilter::ProcessMemory(
    uint8_t** buffer,
    int* size)
{
    if (size <= 0)
    {
        throw MetadataFilter::Exception("Invalid buffer size");
    }

    uint8_t *data = *buffer;
    uint64_t data_size = *size;

    uint64_t position = 0;
    while (position < data_size - 8)
    {
        uint32_t atom_size = (uint32_t)BE_32(&data[position]);
        std::string atom_type = std::string("") +
            static_cast<char>(data[position + 4]) +
            static_cast<char>(data[position + 5]) +
            static_cast<char>(data[position + 6]) +
            static_cast<char>(data[position + 7]);

        if (atom_size + position > data_size ||
            atom_size == 0)
        {
            throw MetadataFilter::Exception("Failed to parse");
        }

        if (atom_type == "moov")
        {
            ProcessMoov(data + position, atom_size);
        }

        position += atom_size;
    }
}

//------------------------------------------------------------------------------
bool Mp4MetadataFilter::IsMimeTypeSupported(
    const std::string& mime_type) const
{
    for (int i = 0; mime_types[i] != NULL; ++i)
    {
        if (mime_type == mime_types[i])
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
bool Mp4MetadataFilter::CanProcess(const std::string& path) const
{
    std::ifstream file(path.c_str());
    if (!file.is_open())
    {
        Log(libecap::flXaction | libecap::ilNormal)
            << "can't open mp4 file";
        return false;
    }

    file.seekg(4);
    if (!file.good())
    {
        Log(libecap::flXaction | libecap::ilNormal)
            << "can't seek mp4 file";
        return false;
    }

    char atom[8] = {0};
    file.read(atom, 8);
    if (file.gcount() != 8)
    {
        Log(libecap::flXaction | libecap::ilNormal)
            << "can't read mp4 file";
        return false;
    }

    return IsFtypSupported(atom);
}

//------------------------------------------------------------------------------
bool Mp4MetadataFilter::CanProcess(
    uint8_t* buffer,
    int size) const
{
    if (size < 12)
    {
        return false;
    }

    return IsFtypSupported(reinterpret_cast<char *>(&buffer[4]));
}

//------------------------------------------------------------------------------
bool Mp4MetadataFilter::IsFtypSupported(const char* atom) const
{
    if (atom[0] != 'f' ||
        atom[1] != 't' ||
        atom[2] != 'y' ||
        atom[3] != 'p')
    {
        Log(libecap::flXaction | libecap::ilDebug) << "no ftyp";
        return false;
    }

    const char* atom_type = &atom[4];

    if (atom_type[0] == '3' &&
        atom_type[1] == 'g' &&
        atom_type[2] == 'p')
    {
        return true;
    }

    if (atom_type[0] == 'q' &&
        atom_type[1] == 't')
    {
        return true;
    }

    if (atom_type[0] == 'i' &&
        atom_type[1] == 's' &&
        atom_type[2] == 'o')
    {
        return true;
    }

    if (atom_type[0] == 'M' &&
        atom_type[1] == '4' &&
        atom_type[2] == 'A')
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
void Mp4MetadataFilter::ProcessMoov(
    uint8_t* moov,
    uint64_t size)
{
    if (size < 8)
    {
        throw MetadataFilter::Exception("Invalid moov size");
    }

    uint32_t moov_atom_size = (uint32_t)BE_32(moov);
    std::string moov_atom_type = std::string("") +
        static_cast<char>(moov[4]) +
        static_cast<char>(moov[5]) +
        static_cast<char>(moov[6]) +
        static_cast<char>(moov[7]);

    if (size < moov_atom_size)
    {
        throw MetadataFilter::Exception("Invalid moov size");
    }

    uint64_t position = 8;
    while (position < moov_atom_size)
    {
        uint32_t atom_size = (uint32_t)BE_32(&moov[position]);
        std::string atom_type = std::string("") +
            static_cast<char>(moov[position + 4]) +
            static_cast<char>(moov[position + 5]) +
            static_cast<char>(moov[position + 6]) +
            static_cast<char>(moov[position + 7]);

        if (atom_size + position > moov_atom_size ||
            atom_size == 0)
        {
            throw MetadataFilter::Exception(
                "Failed to parse moov children");
        }

        if (atom_type == "meta" || atom_type == "udta")
        {
            moov[position + 4] = 'f';
            moov[position + 5] = 'r';
            moov[position + 6] = 'e';
            moov[position + 7] = 'e';
            memset(&moov[position + 8], 0, atom_size - 8);
        }

        position += atom_size;
    }
}
