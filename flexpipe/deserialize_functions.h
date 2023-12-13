#pragma once

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/iostreams/stream.hpp>

namespace flexpipe
{
  // serialize function of kernel ports with boost::serialization
  template <typename T>
  bool deserializeDefault(uint8_t *&data, uint32_t &size, void **msg)
  {
    bool received = true;
    T *castedMsg = static_cast<T *>(*msg);

    boost::iostreams::basic_array_source<char> device((const char *)data, size);
    boost::iostreams::stream<boost::iostreams::basic_array_source<char>> dser(device);
    boost::archive::binary_iarchive ia(dser);
    ia >> *castedMsg;

    delete data;
    data = nullptr;
    return true;
  }
}
