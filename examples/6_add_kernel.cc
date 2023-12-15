#include "sample_kernels.h"

class IntermediateKernel : public flexpipe::Kernel
{
public:
  IntermediateKernel(string id) : flexpipe::Kernel(id)
  {
    setName("IntermediateKernel");
    portManager.registerInPortTag("i1", flexpipe::PortDependency::BLOCKING,
                                  flexpipe::deserializeDefault<MsgType>);
    portManager.registerOutPortTag("o1", flexpipe::sendLocalBasicCopy<MsgType>,
                                   flexpipe::serializeVector<MsgType>);
    frequencyManager.setFrequency(1);
  }

  ~IntermediateKernel() {}

  raft::kstatus run() override
  {
    MsgType *outMsg = portManager.getOutputPlaceholder<MsgType>("o1");
    MsgType *msg = portManager.getInput<MsgType>("i1");

    outMsg->data = msg->data;
    outMsg->setHeader("msg", msg->seq, msg->seq, msg->data.size() * sizeof(char));

    portManager.sendOutput("o1", outMsg);

    printf("IntermediateKernel %dth: size %d\n", msg->seq, msg->dataSize);

    if (msg->seq >= MSG_NUM)
      return raft::stop;

    portManager.freeInput("i1", msg);
    frequencyManager.adjust();

    return raft::proceed;
  }
};


int
main()
{
  raft::map pipeline;

  /* 1. Create instances of the given kernels */
  SourceKernel *sourceKernel = new SourceKernel("sourceKernel");
  SinkKernelNB *sinkKernelNB = new SinkKernelNB("sinkKernelNB");
  SinkKernelB *sinkKernelB = new SinkKernelB("sinkKernelB");

  IntermediateKernel *intermediateKernel = new IntermediateKernel("intermediateKernel");

  /* 2. Activate ports that are registered by the kernel developers */
  sourceKernel->portManager.activateOutPortAsLocal<MsgType>("o1", flexpipe::PortDependency::BLOCKING);
  sourceKernel->portManager.duplicateOutPortAsLocal<MsgType>("o1", "o2", flexpipe::PortDependency::BLOCKING);

  sinkKernelNB->portManager.activateInPortAsLocal<MsgType>("i1");
  sinkKernelB->portManager.activateInPortAsLocal<MsgType>("i1");

  intermediateKernel->portManager.activateInPortAsLocal<MsgType>("i1");
  intermediateKernel->portManager.activateOutPortAsLocal<MsgType>("o1", flexpipe::PortDependency::BLOCKING);

  /* 3. It is also possible for a user to change the desired execution frequency of a kernel. */
  sourceKernel->setFrequency(1);
  sinkKernelB->setFrequency(3);
  sinkKernelNB->setFrequency(3);
  intermediateKernel->setFrequency(3);

  pipeline.link(sourceKernel, "o1", intermediateKernel, "i1", 1);
  pipeline.link(sourceKernel, "o2", sinkKernelB, "i1", 1);

  pipeline.link(intermediateKernel, "o1", sinkKernelNB, "i1", 1);

  /* 5. Run the pipeline */
  pipeline.exe();

  return 0;
}
