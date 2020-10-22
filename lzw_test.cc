// -*- mode: c++; -*-

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE streams

#include <cassert>

#include <iostream>

#include <boost/type_index.hpp>
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
namespace data = boost::unit_test::data;

#include <boost/iostreams/filtering_stream.hpp>
namespace io = boost::iostreams;

#include <lzw_input_filter.hh>
#include <lzw_output_filter.hh>

#define UNUSED(x) ((void)x)

int main()
{
    return 0;
}
