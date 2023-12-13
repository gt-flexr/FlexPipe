#ifndef __FLEXPIPE_PORTMANAGER__
#define __FLEXPIPE_PORTMANAGER__

#include <raftinc/port.hpp>
#include <flexpipe/port_interface.h>

namespace flexpipe
{
  class PortManager
  {
  protected:
    // RaftLib Ports for initialization
    Port *inLocalPorts;
    Port *outLocalPorts;

    std::map<std::string, PortInterface *> inPortMap;
    std::map<std::string, PortInterface *> outPortMap;
    std::multimap<std::string, std::string> duplicatedOutPortMap;

  public:
    PortManager(Port *inLocalPorts, Port *outLocalPorts) : inLocalPorts(inLocalPorts),
                                                           outLocalPorts(outLocalPorts) {}

    ~PortManager()
    {
      for (auto const &inPort : inPortMap)
        delete[] inPort.second;
      for (auto const &outPort : outPortMap)
        delete[] outPort.second;
    }

    /***** Port Registration Interface -- Developer *****/
    void registerInPortTag(std::string tag,
                           PortDependency pd,
                           std::function<bool(uint8_t *&, uint32_t &, void **)> deserialize_free = 0)
    {
      inPortMap[tag] = new PortInterface(inLocalPorts, tag);
      inPortMap[tag]->inputSemantics = pd; // inputPortDependency is set by developer...
      inPortMap[tag]->deserialize_free = deserialize_free;
    }

    void registerOutPortTag(std::string tag,
                            std::function<void(PortInterface *, void *)> sendLocalCopyFunc,
                            std::function<bool(void *, uint8_t *&, uint32_t &, bool)> serializeFunc = 0)
    {
      outPortMap[tag] = new PortInterface(outLocalPorts, tag);
      outPortMap[tag]->sendLocalCopy = sendLocalCopyFunc;
      outPortMap[tag]->serialize = serializeFunc;
    }

    /***** Port Activation Interface -- Deployer *****/
    /** Input Port Activation **/
    template <typename T>
    void activateInportAsLocal(const std::string tag)
    {
      if (inPortMap.find(tag) == inPortMap.end())
      {
        inPortMap[tag]->activateAsLocalInput<T>(tag);
        debug_print("Port %s is activated as local", tag.c_str());
      }
      else
      {
        debug_print("Port %s is already activated", tag.c_str());
      }
    }

    template <typename T>
    void activateInportAsShm(const std::string tag, int maxElem, int eachElemMaxSize)
    {
      if (inPortMap.find(tag) == inPortMap.end())
      {
        if (maxElem <= 0) // maxElem is the max number of elements in the shm
          maxElem = 1;
        if (eachElemMaxSize < sizeof(T)) // eachElemMaxSize is the max size of each queued element in the shm
          eachElemMaxSize = sizeof(T);
        inPortMap[tag]->activateAsLocalShmInput(tag, tag, maxElem, eachElemMaxSize);
        debug_print("Port %s is activated as local shm", tag.c_str());
      }
      else
      {
        debug_print("Port %s is already activated", tag.c_str());
      }
    }


    template <typename T>
    void activateInPortAsRemote(RemoteProtocol p, const std::string tag, int bindingPortNum)
    {
      if (inPortMap.find(tag) == inPortMap.end())
        inPortMap[tag]->activateAsRemoteInput(p, bindingPortNum);
      else
        debug_print("Port %s is already activated", tag.c_str());
    }


    /** Output Port Activation **/
    template <typename T>
    void activateOutPortAsLocal(const std::string tag, PortDependency pd)
    {
      if (outPortMap.find(tag) == outPortMap.end())
      {
        outPortMap[tag]->activateAsLocalOutput<T>(tag, pd);
        debug_print("Port %s is activated as local", tag.c_str());
      }
      else
      {
        debug_print("Port %s is already activated", tag.c_str());
      }
    }

    template <typename T>
    void activateOutPortAsShm(const std::string tag, int maxElem, int eachElemMaxSize, PortDependency pd)
    {
      if (outPortMap.find(tag) == outPortMap.end())
      {
        if (maxElem <= 0) // maxElem is the max number of elements in the shm
          maxElem = 1;
        if (eachElemMaxSize < sizeof(T)) // eachElemMaxSize is the max size of each queued element in the shm
          eachElemMaxSize = sizeof(T);
        outPortMap[tag]->activateAsLocalShmOutput(tag, tag, maxElem, eachElemMaxSize, pd);
        debug_print("Port %s is activated as local shm", tag.c_str());
      }
      else
      {
        debug_print("Port %s is already activated", tag.c_str());
      }
    }

    void activateOutPortAsRemote(RemoteProtocol p, const std::string tag, std::string addr, int portNum)
    {
      if (outPortMap.find(tag) == outPortMap.end())
        outPortMap[tag]->activateAsRemoteOutput(p, addr, portNum);
      else
        debug_print("Port %s is already activated", tag.c_str());
    }


    /***** OutPort Duplication -- Deployer *****/
    template <typename T>
    void duplicateOutPortAsLocal(const std::string from, const std::string to, PortDependency pd)
    {
      duplicatedOutPortMap.insert(std::make_pair<std::string, std::string>(from.c_str(), to.c_str()));
      registerOutPortTag(to, outPortMap[from]->sendLocalCopy, outPortMap[from]->serialize);
      activateOutPortAsLocal<T>(to, pd);
    }


    template <typename T>
    void duplicateOutPortAsShm(const std::string from, const std::string to, int shmId, int maxElem, int eachElemMaxSize, PortDependency pd)
    {
      duplicatedOutPortMap.insert(std::make_pair<std::string, std::string>(from.c_str(), to.c_str()));
      registerOutPortTag(to, outPortMap[from]->sendLocalCopy, outPortMap[from]->serialize);
      activateOutPortAsShm<T>(to, shmId, maxElem, eachElemMaxSize, pd);
    }


    template <typename T>
    void duplicateOutPortAsRemote(const std::string from, const std::string to, RemoteProtocol p, std::string addr, int portNum)
    {
      duplicatedOutPortMap.insert(std::make_pair<std::string, std::string>(from.c_str(), to.c_str()));
      registerOutPortTag(to, outPortMap[from]->sendLocalCopy, outPortMap[from]->serialize);
      activateOutPortAsRemote(p, to, addr, portNum);
    }


    /***** Port Interfaces -- Developer *****/
    template <typename T>
    T *getInput(const std::string tag)
    {
      return inPortMap[tag]->getInput<T>();
    }

    template <typename T>
    T *getOutputPlaceholder(const std::string tag)
    {
      return outPortMap[tag]->getOutputPlaceholder<T>();
    }

    template <typename T>
    void sendOutput(std::string tag, T *msg)
    {
      // send dup ports
      auto portRange = duplicatedOutPortMap.equal_range(tag);
      for (auto port = portRange.first; port != portRange.second; ++port)
      {
        switch (outPortMap[port->second]->state)
        {
        case PortState::LOCAL:
          if (outPortMap[port->second]->localChannel == LocalChannel::FLEXPIPE &&
              outPortMap[port->second]->outputSemantics == PortDependency::BLOCKING)
          {
            outPortMap[port->second]->sendLocalCopy(outPortMap[port->second], msg);
          }
          else if (outPortMap[port->second]->localChannel == LocalChannel::FLEXPIPE &&
                   outPortMap[port->second]->outputSemantics == PortDependency::NONBLOCKING)
          {
            if (outPortMap[port->second]->_checkLocalPortFull() == false)
            {
              outPortMap[port->second]->sendLocalCopy(outPortMap[port->second], msg);
            }
          }
          else if (outPortMap[port->second]->localChannel == LocalChannel::FLEXSHM)
          {
            outPortMap[port->second]->_sendOutputToShmWithFree(msg, false);
          }
          break;

        case PortState::REMOTE:
          outPortMap[port->second]->_sendOutputToRemoteWithFree(msg, false);
          break;
        }
      }

      // send original port
      outPortMap[tag]->sendOutput(msg);
    }

    template <typename T>
    void freeInput(std::string tag, T *msg)
    {
      inPortMap[tag]->freeInput(msg);
    }
  };
}

#endif
