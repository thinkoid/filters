// -*- mode: c++; -*-

#ifndef YPDF_IOSTREAMS_LZW_FILTER_BASE_HH
#define YPDF_IOSTREAMS_LZW_FILTER_BASE_HH

#include <defs.hh>
#include <lzw_filter_base.hh>

namespace ypdf::iostreams {

struct lzw_filter_base_t
{
    using parameter_type = size_t;

    constexpr static parameter_type min_bits =   9;
    constexpr static parameter_type max_bits =  12;

    using code_type = size_t;

    constexpr static code_type   eod_code = 256;
    constexpr static code_type first_code = eod_code + 1;

public:
    lzw_filter_base_t()
        : buf{ }, bits{ min_bits }, pending{ }, next{ first_code }
    { }

protected:
    size_t buf, bits, pending, next;
};

} // namespace ypdf::iostreams

#endif // YPDF_IOSTREAMS_LZW_FILTER_BASE_HH
