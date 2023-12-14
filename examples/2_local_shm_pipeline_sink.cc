#include "sample_kernels.h"

int main()
{
  raft::map pipeline;

  SinkKernel *sinkKernel = new SinkKernel("sinkKernel2");

  // This kernel is emulated as another program which is not a part of the pipeline.
  // Another program can be integrated into the pipeline by using the shared-memory-based IPC.
  sinkKernel->portManager.activateInPortAsShm<MsgType>("i1", "2_local_shm", 1, 1000);
  sinkKernel->setLogger("sink", "sink");
  sinkKernel->setFrequency(1);

  std::vector<std::thread> singleKernelThreads;
  vector<flexpipe::Kernel*> singleKernels;
  singleKernels.push_back(sinkKernel);

  for(int i = 0; i < singleKernels.size(); i++)
  {
    std::thread singleKernelThread(flexpipe::runSingleKernel, sinkKernel);
    singleKernelThreads.push_back(std::move(singleKernelThread));
  }

  for(int i = 0; i < singleKernelThreads.size(); i++) singleKernelThreads[i].join();

  return 0;
}
