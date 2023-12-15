#include "sample_kernels.h"

int main()
{
  raft::map pipeline;

  /* 1. Create instances of the given kernels */
  SourceKernel *sourceKernel = new SourceKernel("sourceKernel");
  SinkKernelNB *sinkKernelNB = new SinkKernelNB("sinkKernel1");

  /* 2. Activate ports that are registered by the kernel developers */
  sourceKernel->portManager.activateOutPortAsLocal<MsgType>("o1", flexpipe::PortDependency::BLOCKING);
  sinkKernelNB->portManager.activateInPortAsLocal<MsgType>("i1");

  // The port which is registered by the kernel developer can be duplicated without changing the kernel codes by a kernel user when the kernel is used in a pipeline.
  sourceKernel->portManager.duplicateOutPortAsShm<MsgType>("o1", "o2", "5_local_shm", 1, MSG_SIZE*2, flexpipe::PortDependency::BLOCKING);

  /* 3. Set the desired execution frequency 3 per seconds */
  sourceKernel->setFrequency(1);
  sinkKernelNB->setFrequency(3);

  sourceKernel->setLogger("source", "source");
  sinkKernelNB->setLogger("sink", "sink");

  /* 4. Link kernels that are connected as flexpipe */
  pipeline.link(sourceKernel, "o1", sinkKernelNB, "i1", 1);

  /* 5. Run the pipeline */
  pipeline.exe();

  return 0;
}
