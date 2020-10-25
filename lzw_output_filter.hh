// -*- mode: c++; -*-
// Copyright 2020- Thinkoid, LLC

#ifndef YPDF_IOSTREAMS_LZW_OUTPUT_FILTER_HH
#define YPDF_IOSTREAMS_LZW_OUTPUT_FILTER_HH

#include <defs.hh>

#include <cctype>

#include <map>
#include <string>

#include <boost/iostreams/concepts.hpp>

namespace ypdf::iostreams {

struct lzw_output_filter_t
    : ::boost::iostreams::output_filter, lzw_filter_base_t
{
    explicit lzw_output_filter_t() : table(make_table()), header{ } {  }

    template< typename Sink >
    bool put(Sink &dst, char c) {
        if (!header) {
            namespace bios = ::boost::iostreams;

            bios::put(dst, 0x1F);
            bios::put(dst, 0x9D);
            bios::put(dst, 0x80 | max_bits);

            header = true;
        }

        string += c;

        if (table.end() == table.find(string)) {
            if ((1UL << max_bits) > next)
                table[string] = next++;

            string.pop_back();

            do_put(table.at(string), bits);
            flush(dst, 7);

            if (bits < max_bits && (1UL << bits) < next)
                ++bits;

            string = c;
        }

        return true;
    }

    template< typename Sink >
    void close(Sink &dst) {
        if (!string.empty())
            do_put(table.at(string), bits);

        flush(dst, 0);

        reset();
    }

private:
    static std::map< std::string, size_t >
    make_table() {
        std::map< std::string, size_t > table;

        for (size_t i = 0; i < 256; ++i)
            table[std::string(1, i)] = i;

        return table;
    }

    void do_put(size_t value, size_t bits) {
        buf |= (value << ((sizeof buf << 3) - pending - bits));
        pending += bits;
    }

    template< typename Sink >
    void flush(Sink &dst, size_t threshold) {
        for (; pending > threshold; pending -= (std::min)(8UL, pending)) {
            namespace bios = ::boost::iostreams;

            const auto c = buf >> ((sizeof buf << 3) - 8);
            bios::put(dst, c);

            buf <<= 8;
        }
    }

    void reset() {
        lzw_filter_base_t::reset();

        table = make_table();

        string = { };
        header = { };
    }

private:
    std::map< std::string, size_t > table;
    std::string string;
    bool header;
};

} // namespace ypdf::iostreams

#endif // YPDF_IOSTREAMS_LZW_OUTPUT_FILTER_HH
