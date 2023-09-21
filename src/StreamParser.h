#pragma once
#include <Arduino.h>
#include "DataDispatcherBase.h"

#define STREAM_ESC_SIGN 0xDB
#define STREAM_ESCAPED_START 0xDC
#define STREAM_ESCAPED_END 0xDD
#define STREAM_ESCAPED_ESC 0xDE

namespace PDTL
{
  class StreamParser
  {
  protected:
    PDTL::DataDispatcherBase *m_pCommandHandler;

    // The stream that we are parsing for commands.
    // Protected so that network command handlers (such as TCPCommandHandler)
    // can update with connection. If null, commands are not parsed.
    // Be sure to reset the parser whenever the source is changed.
    Stream *m_pSource;

    // Characters that signal start and end of message.
    // The buffer is reset when a start character is received. Commands
    // are dispatched when the end character is received.
    uint8_t m_chStartOfMessage, m_chEndOfMessage;

  private:
    // A buffer to collect commands. Buffer, and size, is provided.
    uint8_t *const m_pchBuffer;
    const unsigned m_uMaxBufferSize;

    // Index where next character will be placed in the buffer.
    unsigned m_uNextCharacter;

    // timestamp of last receive data block
    long uLastByteReceived = 0;

  protected:
    uint8_t crc8_sum(uint8_t *buf, uint8_t len);

    StreamParser(PDTL::DataDispatcherBase &rCommandHandler, uint8_t *pchReceiveBuffer, unsigned uBufferSize,
                 Stream &rSourceStream = Serial, uint8_t chStartOfMessage = PDTL_STREAM_START_SIGN, uint8_t chEndOfMessage = PDTL_STREAM_END_SIGN);
    StreamParser(uint8_t *pchReceiveBuffer, unsigned uBufferSize);

    void DispatchMessage(uint8_t *pData, uint8_t size);

    uint8_t EscapeData(uint8_t *src, uint8_t *dest, uint8_t size);
    uint8_t UnescapeData(uint8_t *src, uint8_t size);

    void SendByteCommand(const uint8_t uVariableId, bool command, uint8_t uValue);
    void SendValue(const uint8_t uVariableId, uint8_t *Value, uint8_t size, uint8_t datatype);

  public:
    void Process();
    void Reset();

    void SendUInt8(const uint8_t uVariableId, uint8_t uValue);
    void SendUInt16(const uint8_t uVariableId, uint16_t uValue);
    void SendUInt32(const uint8_t uVariableId, uint32_t uValue);
    void SendInt8(const uint8_t uVariableId, int8_t uValue);
    void SendInt16(const uint8_t uVariableId, int16_t uValue);
    void SendInt32(const uint8_t uVariableId, int32_t uValue);
    void SendFloat(const uint8_t uVariableId, float fValue);
    void SendVariable(const uint8_t uVariableId);
    void SendCommand(const uint8_t uVariableId, const uint8_t Command);
    void SendSysCommand(const uint8_t Command);
  };
}
