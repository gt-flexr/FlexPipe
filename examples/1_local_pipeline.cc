#include "sample_kernels.h"

int main()
{
  raft::map pipeline;

  /* 1. Create instances of the given kernels */
  SourceKernel *sourceKernel = new SourceKernel("sourceKernel");
  SinkKernelB *sinkKernelB = new SinkKernelB("sinkKernel");

  /* 2. Activate ports that are registered by the kernel developers */
  // For output, the port dependency is set by a kernel user who wants to configure the whole pipeline.
  sourceKernel->portManager.activateOutPortAsLocal<MsgType>("o1", flexpipe::PortDependency::BLOCKING);
  // For input, the port dependency is set by a kernel developer who knows the dependency of the kernel.
  sinkKernelB->portManager.activateInPortAsLocal<MsgType>("i1");

  /* 3. It is also possible for a user to change the desired execution frequency of a kernel. */
  sourceKernel->setFrequency(1);
  sinkKernelB->setFrequency(3); // Since sinkKernelB has blocking input, its execution is synchronized with the sourceKernel even with the desired frequency of 3.

  sourceKernel->setLogger("source", "source");
  sinkKernelB->setLogger("sink", "sink");

  /* 4. Link kernels that are connected as flexpipe */
  pipeline.link(sourceKernel, "o1", sinkKernelB, "i1", 1);

  /* 5. Run the pipeline */
  pipeline.exe();

  return 0;
}
