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
    lzw_input_filter_t()
        : table(make_table()), cur{ }, prev{ }, pos{ }, header{ }, eof{ }
    { }

    template< typename Source >
    int get(Source &src) {
        if (eof)
            return EOF;

        if (!header) {
            get_header(src);
            header = true;
        }

        if (0 == cur) {
            const size_t code = do_get(src);

            if (size_t(EOF) == code)
                return eof = true, EOF;

            {
                auto iter = table.find(code);

                if (iter == table.end()) {
                    ASSERT(prev && !prev->empty());
                    iter = table.emplace(code, *prev + (*prev)[0]).first;
                }

                cur = &iter->second;
                pos = 0;
            }

            if (bits < max_bits && (1UL << bits) - 1 <= next)
                ++bits;

            if (prev && !prev->empty() && (1UL << max_bits) > next)
                table[next++] = *prev + cur->at(0);

            prev = cur;
        }

        ASSERT(cur && pos < cur->size());
        const int c = static_cast< unsigned char >(cur->at(pos));

        if (++pos >= cur->size()) {
            cur = 0;
            pos = 0;
        }

        return c;
    }

    template< typename Source >
    void close(Source &) {
        reset();
    }

private:
    static std::map< size_t, std::string > make_table() {
        std::map< size_t, std::string > table;

        for (size_t i = 0; i < 256; ++i)
            table[i] = std::string(1UL, i);

        return table;
    }

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

    template< typename Source >
    void get_header(Source &src) {
        namespace bios = ::boost::iostreams;

        int a, b, c;

        if (EOF == (a = bios::get(src)) || a != 0x1F ||
            EOF == (b = bios::get(src)) || b != 0x9D ||
            EOF == (c = bios::get(src)) || c != (0x80 | max_bits))
            throw std::runtime_error("invalid LZW header");
    }

    void reset() {
        lzw_filter_base_t::reset();

        table = make_table();

        cur = prev = 0;
        pos = 0;

        header = eof = false;
    }

private:
    std::map< size_t, std::string > table;

    std::string *cur, *prev;
    std::size_t pos;

    bool header, eof;
};

} // namespace ypdf::iostreams

#endif // YPDF_IOSTREAMS_LZW_INPUT_FILTER_HH
