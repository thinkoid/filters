// -*- mode: c++; -*-
// Copyright 2020- Thinkoid, LLC

#ifndef YPDF_IOSTREAMS_LZW_OUTPUT_FILTER_HH
#define YPDF_IOSTREAMS_LZW_OUTPUT_FILTER_HH

#include <defs.hh>

#include <cctype>
#include <boost/iostreams/concepts.hpp>

namespace ypdf::iostreams {

struct lzw_output_filter_t
    : boost::iostreams::output_filter, lzw_filter_base_t
{
    constexpr static size_t max_bits = 12;

    explicit lzw_output_filter_t() : header() {
        for (size_t i = 0; i < 256; ++i)
            table[std::string(1, i)] = i;
    }

    template< typename Sink >
    bool put(Sink &dst, char c) {
        if (!header) {
            namespace bios = boost::iostreams;

            bios::put(dst, 0x1F);
            bios::put(dst, 0x9D);

            ASSERT(0 == (max_bits & (max_bits - 1)));
            ASSERT(bits <= max_bits);

            bios::put(dst, 0x80 | max_bits);

            header = true;
        }

        string += c;

        if (table.end() == table.find(string)) {
            if ((1UL << max_bits) > next)
                table[s] = next++;

            string.pop_back();

            do_put(table.at(string), bits);
            flush(7);

            if (bits < max_bits && (1UL << bits) < next)
                ++bits;

            string = *iter;
        }
    }

    template< typename Sink >
    void close(Sink &) {
        flush(0);
    }

private:
    void do_put(size_t value, size_t bits) {
        buf |= (value << ((sizeof buf << 3) - pending));
        pending += bits;
    }

    void flush(size_t threshold) {
        for (; pending > threshold; pending -= (std::min)(8UL, pending)) {
            namespace bios = boost::iostreams;

            const auto c = buf >> ((sizeof buf << 3) - 8);
            bios::put(dst, c);

            buf <<= 8;
        }
    }

private:
    std::map< std::string, size_t > table;
    std::string string;
    bool header;
};

} // namespace ypdf::iostreams

#endif // YPDF_IOSTREAMS_LZW_OUTPUT_FILTER_HH
