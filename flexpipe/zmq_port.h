#ifndef __FLEXPIPE_ZMQPORT__
#define __FLEXPIPE_ZMQPORT__

#include <zmq.h>
#include <zmq.hpp>

namespace flexpipe
{
  /**
   * @brief Component for remote port communication based on ZMQ
   */
  class ZMQPort
  {
    zmq::context_t ctx;

  public:
    zmq::socket_t socket;

    ZMQPort();
    ~ZMQPort();

    // Bind to the given port for receiving
    void bind(int port);
    // Connect to the given address and port for sending
    void connect(std::string addr, int port);

    bool receiveMsg(bool inBlock, uint8_t *&outBuffer, uint32_t &outSize);
  };
}

#endif
