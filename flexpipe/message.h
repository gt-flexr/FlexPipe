#ifndef __FLEXPIPE_MESSAGE__
#define __FLEXPIPE_MESSAGE__

#include <bits/stdc++.h>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>

#include <flexpipe/utils.h>

#define FLEXPIPE_MSG_TAG_SIZE 32

namespace flexpipe
{
  template <class T>
  class Message
  {
  public:
    char tag[FLEXPIPE_MSG_TAG_SIZE];
    uint32_t seq;
    double ts;
    uint32_t dataSize;
    T data;

    void printHeader()
    {
      printf("Message Name Tag: %s \n", tag);
      printf("\tseq: %d, ts: %f, dataSize %d\n", seq, ts, dataSize);
    }

    void setHeader(std::string tag, uint32_t seq, double ts, uint32_t dataSize)
    {
      std::strcpy(this->tag, tag.c_str());
      this->seq = seq;
      this->ts = ts;
      this->dataSize = dataSize;
    }

  private:
    friend class boost::serialization::access;
    template <class Archieve>
    void serialize(Archieve &ar, const unsigned int version)
    {
      ar & tag;
      ar & seq;
      ar & ts;
      ar & dataSize;
      ar & data;
    }
  };

  template <typename T>
  void freePrimitiveMsg(void *msg)
  {
    T *castedMessage = static_cast<T *>(msg);
    delete castedMessage;
  }
}

#endif
