#include <cstring>

#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/iostreams/filtering_stream.hpp>
namespace io = boost::iostreams;

#include <ascii85_input_filter.hh>
#include <ascii85_output_filter.hh>

#include <container_source.hh>
#include <container_sink.hh>

namespace io_ = ypdf::iostreams;

int main()
{
#if 1
    {
        io::filtering_istream str;
    }
#elif 0
    {
        io::filtering_istream str;
        str.push(io_::container_source_t< std::string >("Hello, world"));

        for (int c; EOF != (c = io::get(str)); ) ;

        std::cout << str.eof() << str.bad() << str.fail() << "\n";
    }

    {
        std::string buf;

        io::filtering_ostream str;
        str.push(io_::container_sink_t< std::string >(buf));

        str << "Hello, world!";
        io::close(str);

        std::cout << str.eof() << str.bad() << str.fail() << "\n";
    }
#endif // 0

    return 0;
}
