// -*- mode: c++; -*-

#include <cassert>

#include <iostream>

#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filtering_stream.hpp>
namespace io = ::boost::iostreams;

#include <lzw_input_filter.hh>
#include <lzw_output_filter.hh>

#define UNUSED(x) ((void)x)

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;

    {
        io::filtering_istream in;

        in.push(ypdf::iostreams::lzw_input_filter_t());
        in.push(::io::file_source(argv[1]));

        for (int c; EOF != (c = ::io::get(in)); )
            std::cout.put(c);
    }

    return 0;
}
