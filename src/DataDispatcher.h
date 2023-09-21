#pragma once

#include "DataDispatcherBase.h"

template <int MAX_COMMANDS = 5, int MAX_VARIABLES = 8>
class DataDispatcher : public PDTL::DataDispatcherBase
{
  // Array of commands we can receive and process.
  PDTL::CommandData m_Commands[MAX_COMMANDS];

  // Array of variables we can send and receive
  PDTL::VariableData m_Variables[MAX_VARIABLES];

public:
  DataDispatcher() : DataDispatcherBase(m_Commands, MAX_COMMANDS, m_Variables, MAX_VARIABLES)
  {
  }
};