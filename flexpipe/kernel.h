#ifndef __FLEXPIPE_KERNEL__
#define __FLEXPIPE_KERNEL__

#include <bits/stdc++.h>
#include <iostream>
#include <raft>
#include <chrono>
#include <thread>

#include <raftinc/rafttypes.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <unistd.h>

#include <flexpipe/port_manager.h>
#include <flexpipe/logger.h>
#include <flexpipe/frequency_manager.h>

namespace flexpipe
{
  class Kernel : public raft::kernel
  {
  protected:
    std::string name, id;
    Logger logger;
    FrequencyManager frequencyManager;

  public:
    PortManager portManager;

    Kernel() : portManager(&input, &output)
    {
      id = "no_id";
      name = "FleXRKernel";
    }

    ~Kernel(){};

    Kernel(std::string id) : Kernel()
    {
      this->id = id;
    }

    std::string getId()
    {
      return id;
    }

    void setName(std::string newName)
    {
      name = newName;
    }

    std::string getName()
    {
      return name;
    }

    // run() should be implemented by each kernel's functionalitiy.
    virtual raft::kstatus run() { return raft::stop; }

    void setLogger(std::string loggerID, std::string logFileName)
    {
      logger.set(loggerID, logFileName);
    }

    void setFrequency(int targetFrequency)
    {
      frequencyManager.setFrequency(targetFrequency);
    }
  };

  static void runPipeline(raft::map *pipeline)
  {
    pipeline->exe();
  }

  static void runSingleKernel(Kernel *kernel)
  {
    while (1)
    {
      raft::kstatus ret = kernel->run();
      if (ret == raft::stop)
        break;
    }
  }
}

#endif
