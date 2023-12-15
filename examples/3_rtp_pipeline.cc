#include "sample_kernels.h"

/* Same pipeline with 1_local_pipeline.cc, but just activate the ports with RTP */
// The main purpose of supporting multiple protocols is to reflect the communication semantics between kernels.

int main()
{
  /* 1. Create instances of the given kernels */
  SourceKernel *sourceKernel = new SourceKernel("sourceKernel");
  SinkKernelB *sinkKernelB = new SinkKernelB("sinkKernel");

  /* 2. Activate ports that are registered by the kernel developers */
  sourceKernel->portManager.activateOutPortAsRemote(flexpipe::RemoteProtocol::UVG_RTP, "o1", "127.0.0.1", 5555);
  sinkKernelB->portManager.activateInPortAsRemote(flexpipe::RemoteProtocol::UVG_RTP, "i1", 5555);

  /* 3. It is also possible for a user to change the desired execution frequency of a kernel. */
  sourceKernel->setFrequency(1);
  sinkKernelB->setFrequency(3); // since sinkKernelB has blocking input, its execution is nsynchronized with the sourceKernel.

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

/* If the example is segfaulted, please check the UDP buffer size: */
// $ sysctl net.core.rmem_max
// $ sysctl net.core.rmem_default
//  To increase the size, run the following commands:
// sudo sysctl -w net.core.rmem_max=26214400
// sudo sysctl -w net.core.rmem_default=26214400
