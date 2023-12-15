#pragma once

#include <bits/stdc++.h>
#include <flexpipe/FlexPipe.h>

#define MSG_SIZE (1920*1080*3) // Assume 1080p image
using namespace std;

using MsgType = flexpipe::Message<vector<char>>;

class SourceKernel : public flexpipe::Kernel
{
  int msgIndex = 0;

public:
  SourceKernel(string id) : flexpipe::Kernel(id)
  {
    setName("SourceKernel");
    portManager.registerOutPortTag("o1", flexpipe::sendLocalBasicCopy<MsgType>,
                                   flexpipe::serializeVector<MsgType>);
    frequencyManager.setFrequency(1);
  }

  ~SourceKernel() {}

  raft::kstatus run() override
  {
    if (msgIndex >= 10)
      return raft::stop;
    MsgType *msg = portManager.getOutputPlaceholder<MsgType>("o1");

    msg->data = vector<char>(MSG_SIZE, 1);
    msg->data[50] = msgIndex;
    msgIndex += 1;
    msg->setHeader("msg", msgIndex, getTsMs(), msg->data.size() * sizeof(char));

    portManager.sendOutput("o1", msg);

    printf("SourceKernel %dth: size %d\n", msg->seq, msg->dataSize);

    frequencyManager.adjust();
    return raft::proceed;
  }
};


class SinkKernelB : public flexpipe::Kernel
{
public:
  SinkKernelB(string id) : flexpipe::Kernel(id)
  {
    setName("SinkKernelB");
    portManager.registerInPortTag("i1", flexpipe::PortDependency::BLOCKING,
                                  flexpipe::deserializeDefault<MsgType>);
    frequencyManager.setFrequency(1);
  }

  ~SinkKernelB() {}

  raft::kstatus run() override
  {
    MsgType *msg = portManager.getInput<MsgType>("i1");

    if(msg != nullptr)
    {
      printf("SinkKernel %dth, E2E latency(%.2fms): size %d, 50th elem %d\n", msg->seq, getTsMs() - msg->ts, msg->dataSize, msg->data[50]);
      if (msg->seq >= 10)
        return raft::stop;
    }
    else printf("SinkKernelB did not get any input...\n");

    frequencyManager.adjust();
    portManager.freeInput("i1", msg);

    return raft::proceed;
  }
};


class SinkKernelNB : public flexpipe::Kernel
{
public:
  SinkKernelNB(string id) : flexpipe::Kernel(id)
  {
    setName("SinkKernelNB");
    portManager.registerInPortTag("i1", flexpipe::PortDependency::NONBLOCKING,
                                  flexpipe::deserializeDefault<MsgType>);
    frequencyManager.setFrequency(1);
  }

  ~SinkKernelNB() {}

  raft::kstatus run() override
  {
    MsgType *msg = portManager.getInput<MsgType>("i1");

    if(msg != nullptr)
    {
      printf("SinkKernel %dth, E2E latency(%.2fms): size %d, 50th elem %d\n", msg->seq, getTsMs() - msg->ts, msg->dataSize, msg->data[50]);
      if (msg->seq >= 10)
        return raft::stop;
    }
    else printf("SinkKernelNB did not get any input...\n");

    frequencyManager.adjust();
    portManager.freeInput("i1", msg);

    return raft::proceed;
  }
};