#ifndef __FLEXPIPE_PORT_INTERFACE__
#define __FLEXPIPE_PORT_INTERFACE__

#include <bits/stdc++.h>
#include <raftinc/port.hpp>
#include <flexr_shmq.h>

#include <flexpipe/zmq_port.h>
#include <flexpipe/rtp_port.h>
#include <flexpipe/utils.h>

namespace flexpipe
{
  enum PortDependency
  {
    BLOCKING,
    NONBLOCKING,
  };

  enum PortState
  {
    LOCAL,
    REMOTE,
  };

  enum LocalChannel
  {
    FLEXPIPE,
    FLEXSHM,
  };

  enum RemoteProtocol
  {
    ZMQ_TCP,
    UVG_RTP,
  };

  // FlexPipe PortInterface for FlexPipe Kernel and PortManager
  class PortInterface
  {
  public:
    bool activated;  // is activated?
    std::string tag; // port interface tag

    /* Actual communication ports */
    Port *raftlibPort;         // zero-copy local port across the pipeline
    FlexrShmQueueMeta shmPort; // shared memory port for IPC
    ZMQPort tcpPort;           // ZMQ TCP port
    RtpPort rtpPort;           // UvgRTP port over UDP

    // Port configuration
    PortState state;           // local or remote
    LocalChannel localChannel; // flexpipe or flexshm
    RemoteProtocol protocol;   // zmq_tcp or uvg_rtp

    PortDependency inputSemantics;
    PortDependency outputSemantics;

    // Port functions
    std::function<void(PortInterface *, void *)> sendLocalCopy;

    // Serialization function(void* msg, uint8_t*& serializedData, uint32_t& serializedSize, bool freeMsgData)
    std::function<bool(void *, uint8_t *&, uint32_t &, bool)> serialize;

    // Deserialization function(uint8_t* serializedData, uint32_t serializedSize, void** deserializedMsg)
    std::function<bool(uint8_t *&, uint32_t &, void **)> deserialize_free;

    PortInterface(Port *raftlibPort, std::string tag) : raftlibPort(raftlibPort), tag(tag), activated(false)
    {
      // Default Settings -- Local
      state = PortState::LOCAL;
      localChannel = LocalChannel::FLEXPIPE;
      inputSemantics = PortDependency::BLOCKING;
      outputSemantics = PortDependency::BLOCKING;

      // Default Setting -- remote
      protocol = RemoteProtocol::ZMQ_TCP;

      sendLocalCopy = 0;
      serialize = 0;
      deserialize_free = 0;
    }

    // Check if the local ports' buffers are empty
    bool _checkLocalPortEmpty()
    {
      if (localChannel == LocalChannel::FLEXPIPE)
      {
        auto &port((*raftlibPort)[tag]);
        if (port.size() > 0)
          return false;
        else
          return true;
      }
      if (localChannel == LocalChannel::FLEXSHM)
      {
        return shmPort.isEmpty();
      }
      return false;
    }

    // Check if the local ports' buffers are full
    bool _checkLocalPortFull()
    {
      if (localChannel == LocalChannel::FLEXPIPE)
      {
        auto &port((*raftlibPort)[tag]);
        if (port.capacity() == port.size())
          return true;
        else
          return false;
      }
      if (localChannel == LocalChannel::FLEXSHM)
      {
        return shmPort.isFull();
      }
      return false;
    }

    /*************** Activation for Local Pipeline Communication  *************/
    template <typename T>
    void _activateLocalPort(const std::string tag)
    {
      if (activated)
      {
        debug_print("Port %s is already activated.", tag.c_str());
        return;
      }
      raftlibPort->addPort<T>(tag);
      state = PortState::LOCAL;
      localChannel = LocalChannel::FLEXPIPE;
      activated = true;
    }

    void _activateLocalShmPort(const std::string tag, const std::string shmId, int maxElem, int elemMaxSize)
    {
      if (activated)
      {
        debug_print("Port %s is already activated.", tag.c_str());
        return;
      }
      shmPort.initQueue(shmId.c_str(), maxElem, elemMaxSize);
      state = PortState::LOCAL;
      localChannel = LocalChannel::FLEXSHM;
      activated = true;
    }

    // Input Interface Activation
    template <typename T>
    void activateAsLocalInput(const std::string tag)
    {
      _activateLocalPort<T>(tag);
    }

    void activateAsLocalShmInput(const std::string tag, const std::string shmId, int maxElem, int elemMaxSize)
    {
      _activateLocalShmPort(tag, shmId, maxElem, elemMaxSize);
    }

    // Output Interface Activation
    template <typename T>
    void activateAsLocalOutput(const std::string tag, PortDependency semantics)
    {
      _activateLocalPort<T>(tag);
      outputSemantics = semantics;
    }

    void activateAsLocalShmOutput(const std::string tag, const std::string shmId, int maxElem, int elemMaxSize, PortDependency semantics)
    {
      _activateLocalShmPort(tag, shmId, maxElem, elemMaxSize);
      outputSemantics = semantics;
    }

    /*************** Activation for Remote Pipeline Communication  *************/
    // Input Interface Activation
    void activateAsRemoteInput(RemoteProtocol p, int portNumber)
    {
      if (activated)
      {
        debug_print("Port %s is already activated.", tag.c_str());
        return;
      }

      if (p == RemoteProtocol::ZMQ_TCP)
      {
        tcpPort.bind(portNumber);
        protocol = RemoteProtocol::ZMQ_TCP;
      }
      else if (p == RemoteProtocol::UVG_RTP)
      {
        rtpPort.init("127.0.0.1", portNumber, -1);
        protocol = RemoteProtocol::UVG_RTP;
      }
      else
      {
        debug_print("Unknown protocol type");
        return;
      }

      state = PortState::REMOTE;
      activated = true;
      debug_print("Port %s is activated", tag.c_str());
    }

    // Output Interface Activation
    void activateAsRemoteOutput(RemoteProtocol p, const std::string addr, int portNumber)
    {
      if (activated)
      {
        debug_print("Port %s is already activated.", tag.c_str());
        return;
      }

      if (p == RemoteProtocol::ZMQ_TCP)
      {
        tcpPort.connect(addr, portNumber);
        protocol = RemoteProtocol::ZMQ_TCP;
      }
      else if (p == RemoteProtocol::UVG_RTP)
      {
        rtpPort.init(addr, -1, portNumber);
        protocol = RemoteProtocol::UVG_RTP;
      }
      else
      {
        debug_print("Unknown protocol type");
        return;
      }

      state = PortState::REMOTE;
      activated = true;
      debug_print("Port %s is activated", tag.c_str());
    }

    /*************** Integrated getter to get input from activated remote/local port  *************/
    template <typename T>
    T *getInput()
    {
      if (!activated)
      {
        debug_print("Cannot getInput with inactivated port %s", tag.c_str());
        return nullptr;
      }

      T *input = nullptr;
      switch (state)
      {
      case PortState::LOCAL:
        // Flexpipe's local port with blocking/nonblocking semantics
        if (localChannel == LocalChannel::FLEXPIPE && inputSemantics == PortDependency::BLOCKING)
        {
          T &temp = (*raftlibPort)[tag].peek<T>();
          input = &temp;
        }
        else if (localChannel == LocalChannel::FLEXPIPE && inputSemantics == PortDependency::NONBLOCKING)
        {
          if (_checkLocalPortEmpty() == false)
          {
            T &temp = (*raftlibPort)[tag].peek<T>();
            input = &temp;
          }
        }
        // IPC via flexer_shmq with blocking/nonblocking semantics
        else if (localChannel == LocalChannel::FLEXSHM)
        {
          bool received = _getInputFromShm(input);
          if (received == false && input != nullptr)
          {
            delete input;
            input = nullptr;
          }
        }
        break;

      case PortState::REMOTE:
        bool received = _getInputFromRemote(input);
        if (received == false && input != nullptr) // for failure
        {
          delete input;
          input = nullptr;
        }
        break;
      }

      return input;
    }


    template <typename T>
    bool _getInputFromShm(T *&inputMsg)
    {
      uint8_t *recvBuf = nullptr;
      bool isBlocking = true;
      bool received = false;

      if (inputSemantics != PortDependency::BLOCKING)
        isBlocking = false;

      recvBuf = new uint8_t[shmPort.elemSize];
      uint32_t bufSize = shmPort.elemSize;
      uint32_t recvSize = 0;
      received = shmPort.dequeueElem(recvBuf, recvSize, bufSize, isBlocking);

      // deserialize need to set inputMsg->data properly & free recvBuf
      if (deserialize_free != 0 && received)
      {
        inputMsg = new T;
        received = deserialize_free(recvBuf, recvSize, (void **)&inputMsg);
      }
      // primitive msg (no need deser)
      else if (deserialize_free == 0 && received && recvSize == sizeof(T))
      {
        inputMsg = (T *)recvBuf;
        recvBuf = nullptr;
      }
      else
        debug_print("nothing received...");

      if (recvBuf != nullptr)
        delete recvBuf;
      return received;
    }


    template <typename T>
    bool _getInputFromRemote(T *&inputMsg)
    {
      uint8_t *recvBuf = nullptr;
      uint32_t recvSize = 0;
      bool isBlocking = true;
      bool received = false;

      if (inputSemantics != PortDependency::BLOCKING)
        isBlocking = false;

      switch (protocol)
      {
      case RemoteProtocol::ZMQ_TCP:
        received = tcpPort.receiveMsg(isBlocking, recvBuf, recvSize);
        break;
      case RemoteProtocol::UVG_RTP:
        received = rtpPort.receiveMsg(isBlocking, recvBuf, recvSize);
        break;
      }

      // deserialize_free need to set inputMsg->data properly & free recvBuf
      if (deserialize_free != 0 && received)
      {
        inputMsg = new T;
        received = deserialize_free(recvBuf, recvSize, (void **)&inputMsg);
      }
      // primitive msg (no need deser)
      else if (deserialize_free == 0 && received && recvSize == sizeof(T))
      {
        inputMsg = (T *)recvBuf;
        recvBuf = nullptr;
      }
      else
        debug_print("nothing received...");

      if (recvBuf != nullptr)
        delete recvBuf;
      return received;
    }


    /*************** Integrated getter to get output from activated remote/local port  *************/
    template <typename T>
    T *getOutputPlaceholder()
    {
      if (!activated)
      {
        debug_print("Cannot getOutputPlaceholder with inactivated port %s", tag.c_str());
        return nullptr;
      }

      T *outputPlaceholder = nullptr;

      if (state == PortState::LOCAL)
      {
        if (localChannel == LocalChannel::FLEXPIPE)
        {
          T &temp = (*raftlibPort)[tag].allocate<T>();
          outputPlaceholder = &temp;
        }
        else if (localChannel == LocalChannel::FLEXSHM)
        {
          outputPlaceholder = new T;
        }
      }
      else if (state == PortState::REMOTE)
      {
        outputPlaceholder = new T;
      }

      return outputPlaceholder;
    }


    /*************** Integrated sender to send output via activated remote/local port  *************/
    template <typename T>
    void sendOutput(T *msg)
    {
      if (!activated)
      {
        debug_print("Cannot sendOutput with inactivated port %s", tag.c_str());
        return;
      }

      switch (state)
      {
      case PortState::LOCAL:
        if (localChannel == LocalChannel::FLEXPIPE && outputSemantics == PortDependency::BLOCKING)
          (*raftlibPort)[tag].send();
        else if (localChannel == LocalChannel::FLEXPIPE && outputSemantics == PortDependency::NONBLOCKING && _checkLocalPortFull() == false)
          (*raftlibPort)[tag].send();
        else if (localChannel == LocalChannel::FLEXSHM)
          _sendOutputToShmWithFree(msg, true); // need to free msg after sending
        break;

      case PortState::REMOTE:
        _sendOutputToRemoteWithFree(msg, true); // need to free msg after sending
        break;
      }
    }


    template <typename T>
    void _sendOutputToShmWithFree(T *outputMsg, bool freeMsg)
    {
      uint8_t *sendBuf = nullptr;
      uint32_t sendSize = 0;

      if (serialize != 0)
      {
        // serialize need to free output->data when freeMsgData==true
        bool freeMsgData = freeMsg;
        serialize((void *)outputMsg, sendBuf, sendSize, freeMsgData);
      }
      else
      {
        // static msg: ex) T = Message<int>, Message<int[100]> ...
        sendBuf = (uint8_t *)outputMsg;
        sendSize = sizeof(T);
      }

      if (sendSize > shmPort.elemSize)
      {
        debug_print("shm maxElemSize (%d) is less than msg sendSize (%d).", shmPort.elemSize, sendSize);
        if (serialize && sendBuf != nullptr)
          delete sendBuf;
        if (freeMsg && outputMsg != nullptr)
          delete outputMsg;
        return;
      }

      if (outputSemantics == PortDependency::BLOCKING)
        shmPort.enqueueElem(sendBuf, sendSize, true);
      else
        shmPort.enqueueElem(sendBuf, sendSize, false);
      if (serialize && sendBuf != nullptr)
        delete sendBuf;
      if (freeMsg && outputMsg != nullptr)
        delete outputMsg;
    }


    template <typename T>
    void _sendOutputToRemoteWithFree(T *outputMsg, bool freeMsg)
    {
      uint8_t *sendBuf = nullptr;
      uint32_t sendSize = 0;

      if (serialize != 0)
      {
        // serialize need to free output->data when freeMsgData==true
        bool freeMsgData = freeMsg;
        serialize((void *)outputMsg, sendBuf, sendSize, freeMsgData);
      }
      else
      {
        // static msg: ex) T = Message<int>, Message<int[100]> ...
        sendBuf = (uint8_t *)outputMsg;
        sendSize = sizeof(T);
      }

      if (protocol == RemoteProtocol::ZMQ_TCP)
      {
        zmq::message_t zmqMsg(sendBuf, sendSize);
        tcpPort.socket.send(zmqMsg, zmq::send_flags::none); // relaxed BK with high-water mark
      }

      if (protocol == RemoteProtocol::UVG_RTP)
      {
        rtpPort.send(sendBuf, sendSize, outputMsg->ts); // NBK..., need to make BK
      }

      // delete serialized buffer after sending & conditionally output also
      if (serialize && sendBuf != nullptr)
        delete sendBuf;
      if (freeMsg && outputMsg != nullptr)
        delete outputMsg;
    }


    template <typename T>
    void freeInput(T *msg)
    {
      if (!activated)
      {
        debug_print("Cannot freeInput with inactivated port %s", tag.c_str());
        return;
      }

      if (msg == nullptr)
        return;

      switch (state)
      {
      case PortState::LOCAL:
        if (localChannel == LocalChannel::FLEXPIPE)
          (*raftlibPort)[tag].recycle();
        if (localChannel == LocalChannel::FLEXSHM)
          delete msg;
        break;
      case PortState::REMOTE:
        delete msg;
        break;
      }
    }
  };
}

#endif