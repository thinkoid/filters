// -*- mode: c++; -*-

#ifndef YPDF_IOSTREAMS_LZW_INPUT_FILTER_HH
#define YPDF_IOSTREAMS_LZW_INPUT_FILTER_HH

#include <defs.hh>
#include <lzw_filter_base.hh>

#include <map>
#include <string>

namespace ypdf::iostreams {

struct lzw_input_filter_t
    : ::boost::iostreams::input_filter, lzw_filter_base_t
{
    lzw_input_filter_t() : cur(), prev(), pos(), header(), eof() {
        for (size_t i = 0; i < 256; ++i)
            table[i] = std::string(1UL, i);
    }

    template< typename Source >
    int get(Source &src) {
        if (eof)
            return EOF;

        namespace bios = ::boost::iostreams;

        if (!header) {
            int a, b, c;

            if (EOF == (a = bios::get(src)) || a != 0x1F ||
                EOF == (b = bios::get(src)) || b != 0x9D ||
                EOF == (c = bios::get(src)) || c != (0x80 | max_bits))
                throw std::runtime_error("invalid LZW header");

            header = true;
        }

        if (0 == cur) {
            const size_t code = do_get(src);

            if (size_t(EOF) == code)
                return eof = true, EOF;

            if (table.find(code) == table.end()) {
                ASSERT(prev);

                const auto &s = *prev;
                ASSERT(!s.empty());

                table[code] = s + s[0];
            }

            cur = &table[code];
            pos = 0;

            if (bits < max_bits && (1UL << bits) - 1 <= next)
                ++bits;

            if (prev && !prev->empty() && (1UL << max_bits) > next)
                table[next++] = *prev + cur->at(0);

            prev = cur;
        }

        ASSERT(pos < cur->size());
        const int c = static_cast< unsigned char >(cur->at(pos));

        if (++pos >= cur->size()) {
            cur = 0;
            pos = 0;
        }

        return c;
    }

    template< typename Source >
    void close(Source &) {
        cur = prev = 0;
        pos = 0;
        eof = true;
    }

private:
    template< typename Source >
    size_t do_get(Source &src) {
        namespace bios = ::boost::iostreams;

        for (; pending < bits; pending += 8) {
            const int c = bios::get(src);

            if (EOF == c)
                return EOF;

            buf |= size_t(c) << ((sizeof buf << 3) - pending - 8);
        }

        const size_t code = buf >> ((sizeof buf << 3) - bits);

        if (eod_code == code)
            return EOF;

        buf <<= bits;
        pending -= bits;

        return code;
    }

private:
    std::map< size_t, std::string > table;

    std::string *cur, *prev;
    std::size_t pos;

    bool header, eof;
};

} // namespace ypdf::iostreams

#endif // YPDF_IOSTREAMS_LZW_INPUT_FILTER_HH
