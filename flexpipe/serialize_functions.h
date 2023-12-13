#pragma once

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/iostreams/stream.hpp>

namespace flexpipe
{
  // serialize function of kernel ports for defulat types with boost::serialization
  template <typename T>
  bool serializeDefault(void *msg, uint8_t *&data, uint32_t &size, bool freeMsgData)
  {
    T *castedMsg = static_cast<T *>(msg);

    std::string serStr;
    boost::iostreams::back_insert_device<std::string> inserter(serStr);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::string>> s(inserter);
    boost::archive::binary_oarchive oa(s);
    oa << *castedMsg;
    s.flush();

    size = serStr.size();
    data = new uint8_t[size];
    memcpy(data, serStr.data(), size);

    return true;
  }


  // serialize function of kernel ports for vector types with boost::serialization
  template <typename T>
  bool serializeVector(void *msg, uint8_t *&data, uint32_t &size, bool freeMsgData)
  {
    serializeDefault<T>(msg, data, size, freeMsgData);

    if (freeMsgData)
    {
      T *castedMsg = static_cast<T *>(msg);
      castedMsg->data.clear();
    }
    return true;
  }

}
