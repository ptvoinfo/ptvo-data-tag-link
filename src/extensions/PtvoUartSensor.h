#pragma once
#include <Arduino.h>
#include <Stream.h>
#include "DataDispatcherBase.h"
#include "StreamParser.h"

#define MAX_ENDPOINTS 8
#define ENDPOINT_1 PDTL_VARIABLE_1
#define ENDPOINT_2 PDTL_VARIABLE_2
#define ENDPOINT_3 PDTL_VARIABLE_3
#define ENDPOINT_4 PDTL_VARIABLE_4
#define ENDPOINT_5 PDTL_VARIABLE_5
#define ENDPOINT_6 PDTL_VARIABLE_6
#define ENDPOINT_7 PDTL_VARIABLE_7
#define ENDPOINT_8 PDTL_VARIABLE_8

#define PTVO_CLICK_RELEASE 0
#define PTVO_CLICK_SINGLE 1
#define PTVO_CLICK_DOUBLE 2
#define PTVO_CLICK_TRIPLE 3
#define PTVO_CLICK_HOLD 4

namespace PDTL
{
  enum ESysCommand : uint8_t
  {
    Rejoin = 2,
    SendReport = 3,
    StatusHold = 0xE0,
    StatusInit = 0xE1,
    StatusNetDiscovery = 0xE2,
    StatusNetJoining = 0xE3,
    StatusNetRejoinChannel = 0xE4,
    StatusEndDeviceUnauth = 0xE5,
    StatusEndDevice = 0xE6,
    StatusRouter = 0xE7,
    StatusNetOrphan = 0xEA,
    StatusNetKeepAlive = 0xEB,
    StatusNetBackoff = 0xEC,
    StatusNetRejoinAllChannel = 0xED
  };

  enum EEndpointCommand : uint8_t
  {
    GetValue = PDTL_COMMAND_GET_DATA
  };

  template <int MAX_COMMANDS = MAX_ENDPOINTS, int MAX_VARIABLES = MAX_ENDPOINTS>
  class PtvoUartSensor : public PDTL::DataDispatcherBase, public PDTL::StreamParser
  {
    // Array of commands we can receive and process.
    PDTL::CommandData m_Commands[MAX_COMMANDS];

    // Array of variables we can send and receive
    PDTL::VariableData m_Variables[MAX_VARIABLES];

    // Buffer for data received.
    char m_achBuffer[32];

  public:
    PtvoUartSensor(Stream &rSourceStream = Serial, char chStartOfMessage = PDTL_STREAM_START_SIGN, char chEndOfMessage = PDTL_STREAM_END_SIGN)
        : DataDispatcherBase(m_Commands, MAX_COMMANDS, m_Variables, MAX_VARIABLES), StreamParser(*(static_cast<PDTL::DataDispatcherBase *>(this)), m_achBuffer, sizeof(m_achBuffer), rSourceStream, chStartOfMessage, chEndOfMessage)
    {
    }

    void SendSysCommand(ESysCommand uCommandId);
    void SendCommand(uint8_t Endpoint, EEndpointCommand uCommandId);
  };
}