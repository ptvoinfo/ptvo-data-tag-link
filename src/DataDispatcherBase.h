#pragma once
#include <Arduino.h>
#include <stdint.h>

#define PDTL_VARIABLE_1 0
#define PDTL_VARIABLE_2 1
#define PDTL_VARIABLE_3 2
#define PDTL_VARIABLE_4 3
#define PDTL_VARIABLE_5 4
#define PDTL_VARIABLE_6 5
#define PDTL_VARIABLE_7 6
#define PDTL_VARIABLE_8 7
#define PDTL_VARIABLE_9 8
#define PDTL_VARIABLE_10 9
#define PDTL_VARIABLE_11 10
#define PDTL_VARIABLE_12 11
#define PDTL_VARIABLE_13 12
#define PDTL_VARIABLE_14 13
#define PDTL_VARIABLE_15 14
#define PDTL_VARIABLE_16 15

#define PDTL_STREAM_START_SIGN '!'
#define PDTL_STREAM_END_SIGN ';'

#define PDTL_ID_MASK 0x0F
#define PDTL_ID_SYS 0xFE

#define PDTL_COMMAND_GET_DATA 1

#define PDTL_DATA_TYPE_BOOL 0
#define PDTL_DATA_TYPE_UINT8 1
#define PDTL_DATA_TYPE_INT8 2
#define PDTL_DATA_TYPE_UINT16 3
#define PDTL_DATA_TYPE_INT16 4
#define PDTL_DATA_TYPE_UINT32 5
#define PDTL_DATA_TYPE_INT32 6
#define PDTL_DATA_TYPE_UINT64 7
#define PDTL_DATA_TYPE_INT64 8
#define PDTL_DATA_TYPE_FLOAT 9
#define PDTL_DATA_TYPE_DOUBLE 0xA
#define PDTL_DATA_TYPE_STR 0xD
#define PDTL_DATA_TYPE_COMMAND 0xE
#define PDTL_DATA_TYPE_SYS_COMMAND 0xF
#define PDTL_DATA_TYPE_SHIFT 4
#define PDTL_DATA_TYPE_MASK 0xF0

namespace PDTL
{
  struct VariableData;
  struct CommandData;
  typedef bool (*VariableProcessProc_t)(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize);
  typedef void (*VariableCallback_t)(VariableData &rVariableInfo, bool bIsWrite);
  typedef void (*CommandCallback_t)(CommandData &rVariableInfo);
  typedef void (*DefaultHandler_t)(uint8_t uCommandId, uint8_t uVariableId);

  struct CommandData
  {
    uint8_t m_uId;
    uint8_t m_uVarId;
    CommandCallback_t m_fnCallback;
  };

  struct VariableData
  {
    uint8_t m_uId;
    uint8_t m_uDatatype;
    void *m_pVariable;
    uint8_t m_uMaxBufferSize;
    VariableCallback_t m_fnCallback;
    VariableProcessProc_t m_fnProcessProc;
  };

  class DataDispatcherBase
  {
    // Array of up to m_uMaxCommands we can match & dispatch.
    CommandData *const m_pCommands;
    const uint8_t m_uMaxCommands;
    uint8_t m_uLastCommand;

    VariableData *const m_pVariableData;
    const uint8_t m_uMaxVariables;
    uint8_t m_uLastVariable;

    // Handler called (if not null) when no command matches.
    DefaultHandler_t m_fnDefaultHandler;

  protected:
    DataDispatcherBase(CommandData *pCallbackBuffer, uint8_t uCallbackBufferLength, VariableData *pVariableDataBuffer, uint8_t uVariableDataLength);

  public:
    bool AddCommand(const uint8_t uCommandId, const uint8_t uVariableId, CommandCallback_t CallbackFunction);

    void SetDefaultHandler(DefaultHandler_t CallbackFunction);
    void Clear();

    VariableData *FindVariable(uint8_t uVariableId);
    void DispatchCommand(uint8_t uCommandId, uint8_t uVariableId, Print &rSource);
    void DispatchVariable(uint8_t uVariableId, Print &rSource, uint8_t *buffer, uint8_t bufferSize);

    bool AddVariable(uint8_t uVariableId, uint8_t &rVariable, VariableCallback_t CallbackFunction = NULL);
    bool AddVariable(uint8_t uVariableId, uint16_t &rVariable, VariableCallback_t CallbackFunction = NULL);
    bool AddVariable(uint8_t uVariableId, uint32_t &rVariable, VariableCallback_t CallbackFunction = NULL);
    bool AddVariable(uint8_t uVariableId, int8_t &rVariable, VariableCallback_t CallbackFunction = NULL);
    bool AddVariable(uint8_t uVariableId, int16_t &rVariable, VariableCallback_t CallbackFunction = NULL);
    bool AddVariable(uint8_t uVariableId, int32_t &rVariable, VariableCallback_t CallbackFunction = NULL);
    bool AddVariable(uint8_t uVariableId, float &rVariable, VariableCallback_t CallbackFunction = NULL);
    bool AddVariable(uint8_t uVariableId, double &rVariable, VariableCallback_t CallbackFunction = NULL);
    bool AddVariable(uint8_t uVariableId, char *pchBuffer, uint8_t uMaxBufferSize, VariableCallback_t CallbackFunction = NULL);
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    bool AddVariable(uint8_t uVariableId, unsigned long &rVariable, VariableCallback_t CallbackFunction = NULL);
    bool AddVariable(uint8_t uVariableId, long &rVariable, VariableCallback_t CallbackFunction = NULL);
#endif
  protected:
    static bool ProcessVariable_uint8(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize);
    static bool ProcessVariable_uint16(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize);
    static bool ProcessVariable_uint32(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize);
    static bool ProcessVariable_int8(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize);
    static bool ProcessVariable_int16(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize);
    static bool ProcessVariable_int32(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize);
    static bool ProcessVariable_float(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize);
    static bool ProcessVariable_double(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize);
    static bool ProcessVariable_string(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize);
    bool AddVariable(uint8_t uVariableId, void *pVariable, uint8_t uDatatype, VariableProcessProc_t ProcessFunction, VariableCallback_t CallbackFunction);
  };
}