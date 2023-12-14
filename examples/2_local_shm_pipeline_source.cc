#include "sample_kernels.h"

int main()
{
  raft::map pipeline;

  /* 1. Create instances of the given kernels */
  SourceKernel *sourceKernel = new SourceKernel("sourceKernel");
  SinkKernel *sinkKernel = new SinkKernel("sinkKernel1");

  /* 2. Activate ports that are registered by the kernel developers */
  sourceKernel->portManager.activateOutPortAsLocal<MsgType>("o1", flexpipe::PortDependency::NONBLOCKING);
  sinkKernel->portManager.activateInPortAsLocal<MsgType>("i1");

  // The port which is registered by the kernel developer can be duplicated without changing the kernel codes by a kernel user when the kernel is used in a pipeline.
  sourceKernel->portManager.duplicateOutPortAsShm<MsgType>("o1", "o2", "2_local_shm", 1, 1000, flexpipe::PortDependency::BLOCKING);

  /* 3. Set the desired execution frequency 3 per seconds */
  sourceKernel->setFrequency(3);
  sinkKernel->setFrequency(3);

  sourceKernel->setLogger("source", "source");
  sinkKernel->setLogger("sink", "sink");

  /* 4. Link kernels that are connected as flexpipe */
  pipeline.link(sourceKernel, "o1", sinkKernel, "i1", 10);

  /* 5. Run the pipeline */
  pipeline.exe();

  return 0;
}
