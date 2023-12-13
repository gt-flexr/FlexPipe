#pragma once

#include <flexpipe/message.h>

namespace flexpipe
{
  template <typename T>
  void sendLocalBasicCopy(PortInterface *port, void *msg)
  {
    T *castedMessage = static_cast<T*>(msg);
    T *copiedMessage = port->getOutputPlaceholder<T>();

    copiedMessage->setHeader(castedMessage->tag, castedMessage->seq, castedMessage->ts, castedMessage->dataSize);
    copiedMessage->data = castedMessage->data;

    port->sendOutput(copiedMessage);
  }

  static void sendLocalPointerMsgCopy(PortInterface *port, void *msg)
  {
    Message<uint8_t *> *castedMessage = static_cast<Message<uint8_t *> *>(msg);
    Message<uint8_t *> *copiedMessage = port->getOutputPlaceholder<Message<uint8_t *>>();

    copiedMessage->setHeader(castedMessage->tag, castedMessage->seq, castedMessage->ts, castedMessage->dataSize);
    copiedMessage->data = new uint8_t[copiedMessage->dataSize];
    memcpy(copiedMessage->data, castedMessage->data, castedMessage->dataSize);
    port->sendOutput(copiedMessage);
  }
}
