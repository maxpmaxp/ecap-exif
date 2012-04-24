#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <iostream>
#include <libecap/common/log.h>

namespace ExifAdapter
{

class Log
{
public:
    Log(const libecap::LogVerbosity& verbosity);
    ~Log();

    template <class T>
    const Log& operator <<(const T &msg) const
        {
            if (debug_stream)
            {
                *debug_stream  << msg;
            }

            return *this;
        }

private:
    Log(const Log&);
    Log &operator=(const Log&);

    std::ostream *debug_stream;
};

}

#endif
