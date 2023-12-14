#include <flexpipe/zmq_port.h>

namespace flexpipe
{
  ZMQPort::ZMQPort()
  {
    socket = zmq::socket_t(ctx, zmq::socket_type::pair);
  }

  ZMQPort::~ZMQPort()
  {
    socket.close();
    ctx.shutdown();
    ctx.close();
  }

  void ZMQPort::bind(int port)
  {
    int rcvhwm = 10; // in-bound high watermark
    assert(0 == zmq_setsockopt(socket, ZMQ_RCVHWM, &rcvhwm, sizeof(rcvhwm)));
    socket.bind("tcp://*:" + std::to_string(port));
  }

  void ZMQPort::connect(std::string addr, int port)
  {
    int sndhwm = 10; // out-bound high watermark
    assert(0 == zmq_setsockopt(socket, ZMQ_SNDHWM, &sndhwm, sizeof(sndhwm)));
    socket.connect("tcp://" + addr + ":" + std::to_string(port));
  }

  bool ZMQPort::receiveMsg(bool inBlock, uint8_t *&outBuffer, uint32_t &outSize)
  {
    zmq::message_t msg;

    if (inBlock)
      socket.recv(msg, zmq::recv_flags::none);
    else
      socket.recv(msg, zmq::recv_flags::dontwait);

    if (msg.size() > 0)
    {
      outSize = msg.size();
      outBuffer = new uint8_t[outSize];
      memcpy(outBuffer, msg.data(), outSize);

      return true;
    }

    return false;
  }
}
