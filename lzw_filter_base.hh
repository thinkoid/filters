// -*- mode: c++; -*-

#ifndef YPDF_IOSTREAMS_LZW_INPUT_FILTER_HH
#define YPDF_IOSTREAMS_LZW_INPUT_FILTER_HH

#include <defs.hh>
#include <lzw_filter_base.hh>

namespace ypdf::iostreams {

struct lzw_filter_base_t
{
    constexpr static size_t min_bits =  9;
    constexpr static size_t max_bits = 12;

    constexpr static size_t clear_code = 256;
    constexpr static size_t   eod_code = 257;

public:
    lzw_filter_base_t()
        : buf(), bits(min_bits), pending(), next(eod_code + 1)
    { }

protected:
    size_t buf, bits, pending, next;
};

} // namespace ypdf::iostreams

#endif // YPDF_IOSTREAMS_LZW_INPUT_FILTER_HH
