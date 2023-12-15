#include "sample_kernels.h"

int main()
{
  raft::map pipeline;

  SinkKernelB *sinkKernelB = new SinkKernelB("sinkKernel2");

  // This kernel is emulated as another program which is not a part of the pipeline.
  // Another program can be integrated into the pipeline by using the shared-memory-based IPC.
  sinkKernelB->portManager.activateInPortAsShm<MsgType>("i1", "5_local_shm", 1, MSG_SIZE*2);
  sinkKernelB->setLogger("sink", "sink");
  sinkKernelB->setFrequency(3);

  std::vector<std::thread> singleKernelThreads;
  vector<flexpipe::Kernel*> singleKernels;
  singleKernels.push_back(sinkKernelB);

  for(int i = 0; i < singleKernels.size(); i++)
  {
    std::thread singleKernelThread(flexpipe::runSingleKernel, singleKernels[i]);
    singleKernelThreads.push_back(std::move(singleKernelThread));
  }

  for(int i = 0; i < singleKernelThreads.size(); i++) singleKernelThreads[i].join();

  return 0;
}
