#pragma once
#include <Arduino.h>
#include <Stream.h>

#include "DataDispatcherBase.h"
#include "StreamParser.h"

template <int MAX_COMMANDS = 10, int CP_SERIAL_BUFFER_SIZE = 30, int MAX_VARIABLES = 10>
class DataHandler : public PDTL::DataDispatcherBase, public PDTL::StreamParser
{
  // Array of commands we can match & dispatch.
  PDTL::CommandData m_Commands[MAX_COMMANDS];

  // Array of variables we can match & set/print
  PDTL::VariableData m_Variables[MAX_VARIABLES];

  // Buffer for data received.
  uint8_t m_achBuffer[CP_SERIAL_BUFFER_SIZE];

public:
  DataHandler(Stream &rSourceStream = Serial, char chStartOfMessage = PDTL_STREAM_START_SIGN, char chEndOfMessage = PDTL_STREAM_END_SIGN)
      : DataDispatcherBase(m_Commands, MAX_COMMANDS, m_Variables, MAX_VARIABLES), StreamParser(*(static_cast<PDTL::DataDispatcherBase *>(this)), m_achBuffer, sizeof(m_achBuffer), rSourceStream, chStartOfMessage, chEndOfMessage)
  {
  }
};
