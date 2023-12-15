#include "sample_kernels.h"

/* Same pipeline with 1_local_pipeline.cc, but just activate the ports with TCP */
// The main purpose of supporting multiple protocols is to reflect the communication semantics between kernels.

int main()
{
  /* 1. Create instances of the given kernels */
  SourceKernel *sourceKernel = new SourceKernel("sourceKernel");
  SinkKernelB *sinkKernelB = new SinkKernelB("sinkKernel");

  /* 2. Activate ports that are registered by the kernel developers */
  sourceKernel->portManager.activateOutPortAsRemote(flexpipe::RemoteProtocol::ZMQ_TCP, "o1", "127.0.0.1", 5555);
  sinkKernelB->portManager.activateInPortAsRemote(flexpipe::RemoteProtocol::ZMQ_TCP, "i1", 5555);

  /* 3. It is also possible for a user to change the desired execution frequency of a kernel. */
  sourceKernel->setFrequency(1);
  sinkKernelB->setFrequency(3);

  std::vector<std::thread> singleKernelThreads;
  vector<flexpipe::Kernel*> separateKernels;
  separateKernels.push_back(sourceKernel);
  separateKernels.push_back(sinkKernelB);

  for(int i = 0; i < separateKernels.size(); i++)
  {
    std::thread singleKernelThread(flexpipe::runSingleKernel, separateKernels[i]);
    singleKernelThreads.push_back(std::move(singleKernelThread));
  }

  for(int i = 0; i < singleKernelThreads.size(); i++) singleKernelThreads[i].join();


  return 0;
}

