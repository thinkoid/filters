// -*- mode: c++; -*-

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE streams

#include <cassert>

#include <iostream>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
namespace io = ::boost::iostreams;

#include <boost/type_index.hpp>
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
namespace  data = boost::unit_test::data;

#include <lzw_input_filter.hh>
#include <lzw_output_filter.hh>

#include <fmt/format.h>
using fmt::print;

#define UNUSED(x) ((void)x)

BOOST_AUTO_TEST_SUITE(lzw_test)

static std::vector< char >
make_source(size_t n)
{
    std::vector< char > xs(n);
    std::generate(xs.begin(), xs.end(), rand);
    return xs;
}

BOOST_AUTO_TEST_CASE(init)
{
    srand(time(0));
}

BOOST_DATA_TEST_CASE(
    roundtrip,
    data::xrange< size_t >( (data::begin = 1000, data::end=10000, data::step=317)))
{
    std::vector< char > xs(make_source(sample)), ys;

    {
        io::filtering_ostream out;

        out.push(ypdf::iostreams::lzw_output_filter_t());
        out.push(::io::back_insert_device(ys));

        for (const auto &x : xs)
            out.put(x);
    }

    std::vector< char > zs;

    {
        io::filtering_istream in;

        in.push(ypdf::iostreams::lzw_input_filter_t());
        in.push(::io::array_source(ys.data(), ys.size()));

        for (int c; EOF != (c = in.get()); )
            zs.push_back(c);
    }

    BOOST_TEST(xs == zs);
}

BOOST_AUTO_TEST_SUITE_END()
