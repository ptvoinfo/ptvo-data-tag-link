#include "DataDispatcherBase.h"

using namespace PDTL;

DataDispatcherBase::DataDispatcherBase(CommandData *pCallbackBuffer, uint8_t uCallbackBufferLength, VariableData *pVariableDataBuffer, uint8_t uVariableDataLength)
    : m_pCommands(pCallbackBuffer), m_uMaxCommands(uCallbackBufferLength), m_pVariableData(pVariableDataBuffer), m_uMaxVariables(uVariableDataLength)
{
  m_uLastCommand = 0;
  m_uLastVariable = 0;
  m_fnDefaultHandler = NULL;
}

bool DataDispatcherBase::AddCommand(const uint8_t uCommandId, const uint8_t uVariableId, CommandCallback_t CallbackFunction)
{
  if (m_uLastCommand < m_uMaxCommands)
  {
    m_pCommands[m_uLastCommand].m_fnCallback = CallbackFunction;
    m_pCommands[m_uLastCommand].m_uId = uCommandId;
    m_pCommands[m_uLastCommand].m_uVarId = uVariableId;
    ++m_uLastCommand;
    return true;
  }

  Serial.println(F("AddCommand: full"));

  return false; // too many commands stored already.
}

void DataDispatcherBase::SetDefaultHandler(DefaultHandler_t CallbackFunction)
{
  m_fnDefaultHandler = CallbackFunction;
}

void DataDispatcherBase::Clear()
{
  m_uLastCommand = 0;
  m_uLastVariable = 0;
}

bool DataDispatcherBase::AddVariable(uint8_t uVariableId, uint8_t &rVariable, VariableCallback_t CallbackFunction = NULL)
{
  return AddVariable(uVariableId, &rVariable, PDTL_DATA_TYPE_UINT8, ProcessVariable_uint8, CallbackFunction);
}

bool DataDispatcherBase::AddVariable(uint8_t uVariableId, uint16_t &rVariable, VariableCallback_t CallbackFunction = NULL)
{
  return AddVariable(uVariableId, &rVariable, PDTL_DATA_TYPE_UINT16, ProcessVariable_uint16, CallbackFunction);
}

bool DataDispatcherBase::AddVariable(uint8_t uVariableId, uint32_t &rVariable, VariableCallback_t CallbackFunction = NULL)
{
  return AddVariable(uVariableId, &rVariable, PDTL_DATA_TYPE_UINT32, ProcessVariable_uint32, CallbackFunction);
}

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
bool DataDispatcherBase::AddVariable(uint8_t uVariableId, unsigned long &rVariable, VariableCallback_t CallbackFunction = NULL)
{
  return AddVariable(uVariableId, &rVariable, PDTL_DATA_TYPE_UINT32, ProcessVariable_uint32, CallbackFunction);
}

bool DataDispatcherBase::AddVariable(uint8_t uVariableId, long &rVariable, VariableCallback_t CallbackFunction = NULL)
{
  return AddVariable(uVariableId, &rVariable, PDTL_DATA_TYPE_UINT32, ProcessVariable_int32, CallbackFunction);
}
#endif

bool DataDispatcherBase::AddVariable(uint8_t uVariableId, int8_t &rVariable, VariableCallback_t CallbackFunction = NULL)
{
  return AddVariable(uVariableId, &rVariable, PDTL_DATA_TYPE_INT8, ProcessVariable_int8, CallbackFunction);
}

bool DataDispatcherBase::AddVariable(uint8_t uVariableId, int16_t &rVariable, VariableCallback_t CallbackFunction = NULL)
{
  return AddVariable(uVariableId, &rVariable, PDTL_DATA_TYPE_INT16, ProcessVariable_int16, CallbackFunction);
}

bool DataDispatcherBase::AddVariable(uint8_t uVariableId, int32_t &rVariable, VariableCallback_t CallbackFunction = NULL)
{
  return AddVariable(uVariableId, &rVariable, PDTL_DATA_TYPE_INT32, ProcessVariable_int32, CallbackFunction);
}

#if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_SAM) || defined(CORE_TEENSY)
bool DataDispatcherBase::AddVariable(uint8_t uVariableId, int &rVariable, VariableCallback_t CallbackFunction = NULL)
{
  return AddVariable(uVariableId, &rVariable, PDTL_DATA_TYPE_INT32, ProcessVariable_int32, CallbackFunction);
}
#endif

#if defined(ARDUINO_ARCH_RP2040)
bool DataDispatcherBase::AddVariable(uint8_t uVariableId, int &rVariable, VariableCallback_t CallbackFunction = NULL)
{
  return AddVariable(uVariableId, &rVariable, PDTL_DATA_TYPE_INT32, ProcessVariable_int32, CallbackFunction);
}
#endif

bool DataDispatcherBase::AddVariable(uint8_t uVariableId, float &rVariable, VariableCallback_t CallbackFunction = NULL)
{
  return AddVariable(uVariableId, &rVariable, PDTL_DATA_TYPE_FLOAT, ProcessVariable_float, CallbackFunction);
}

bool DataDispatcherBase::AddVariable(uint8_t uVariableId, double &rVariable, VariableCallback_t CallbackFunction = NULL)
{
  return AddVariable(uVariableId, &rVariable, PDTL_DATA_TYPE_DOUBLE, ProcessVariable_double, CallbackFunction);
}

VariableData *DataDispatcherBase::FindVariable(uint8_t uVariableId)
{
  for (uint8_t i = 0; i < m_uLastVariable; i++)
  {
    if (m_pVariableData[i].m_uId == uVariableId)
    {
      return &m_pVariableData[i];
    }
  }
  return NULL;
}

bool DataDispatcherBase::AddVariable(uint8_t uVariableId, char *pchBuffer, uint8_t uMaxBufferSize, VariableCallback_t CallbackFunction = NULL)
{
  VariableData *pVarData = FindVariable(uVariableId);
  if (pVarData == NULL)
  {
    if (m_uLastVariable >= m_uMaxVariables)
    {
      Serial.println(F("AddVariable: full"));
      return false; // too many variables stored already.
    }
    pVarData = &m_pVariableData[m_uLastVariable];
    ++m_uLastVariable;
  }
  pVarData->m_uId = uVariableId;
  pVarData->m_uDatatype = PDTL_DATA_TYPE_STR;
  pVarData->m_pVariable = pchBuffer;
  pVarData->m_uMaxBufferSize = uMaxBufferSize;
  pVarData->m_fnProcessProc = ProcessVariable_string;
  pVarData->m_fnCallback = CallbackFunction;
  return true;
}

static const uint8_t aDataSizeByType[16] = {1, 1, 1, 2, 2, 4, 4, 8, 8, 4, 8, 0, 0, 0, 0, 0};

bool DataDispatcherBase::AddVariable(uint8_t uVariableId, void *pVariable, uint8_t uDatatype, VariableProcessProc_t ProcessFunction, VariableCallback_t CallbackFunction = NULL)
{
  VariableData *pVarData = FindVariable(uVariableId);
  if (pVarData == NULL)
  {
    if (m_uLastVariable >= m_uMaxVariables)
    {
      Serial.println(F("AddVariable: full"));
      return false; // too many variables stored already.
    }
    pVarData = &m_pVariableData[m_uLastVariable];
    ++m_uLastVariable;
  }

  pVarData->m_uId = uVariableId;
  pVarData->m_uDatatype = uDatatype;
  pVarData->m_pVariable = pVariable;
  pVarData->m_fnProcessProc = ProcessFunction;
  pVarData->m_fnCallback = CallbackFunction;
  pVarData->m_uMaxBufferSize = aDataSizeByType[uDatatype];
  return true;
}

void DataDispatcherBase::DispatchCommand(uint8_t uCommandId, uint8_t uVariableId, Print &rSource)
{
  uint8_t uCommand;
  CommandData *pCommand;
  uCommand = m_uLastCommand;
  pCommand = m_pCommands;

  while (uCommand--)
  {
    if ((pCommand->m_uId == uCommandId) && (pCommand->m_uVarId == uVariableId))
    {
      pCommand->m_fnCallback(*pCommand);
      return;
    }

    ++pCommand;
  }

  // No command matched.
  if (m_fnDefaultHandler != NULL)
  {
    (*m_fnDefaultHandler)(uCommandId, uVariableId);
  }
  else
  {
    rSource.println(F("Unk cmd"));
  }
}

void DataDispatcherBase::DispatchVariable(uint8_t uVariableId, Print &rSource, uint8_t *buffer, uint8_t bufferSize)
{
  VariableData *pVariableData = FindVariable(uVariableId);
  if (pVariableData == NULL)
  {
    return;
  }
  if (pVariableData->m_fnProcessProc)
  {
    if (!pVariableData->m_fnProcessProc(*pVariableData, buffer, bufferSize))
    {
      return;
    }
  }
  if (pVariableData->m_fnCallback)
  {
    pVariableData->m_fnCallback(*pVariableData, true);
  }
  return;
}

template <class T>
bool ProcessIntVariable(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize)
{
  if (bufferSize < sizeof(T))
  {
    return false;
  }
  T *pValue = (T *)(rVariableInfo.m_pVariable);
  *pValue = *(T *)(buffer);
  return true;
}

bool DataDispatcherBase::ProcessVariable_uint8(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize)
{
  return ProcessIntVariable<uint8_t>(rVariableInfo, buffer, bufferSize);
}

bool DataDispatcherBase::ProcessVariable_uint16(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize)
{
  return ProcessIntVariable<uint16_t>(rVariableInfo, buffer, bufferSize);
}

bool DataDispatcherBase::ProcessVariable_uint32(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize)
{
  return ProcessIntVariable<uint32_t>(rVariableInfo, buffer, bufferSize);
}

bool DataDispatcherBase::ProcessVariable_int8(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize)
{
  return ProcessIntVariable<int8_t>(rVariableInfo, buffer, bufferSize);
}

bool DataDispatcherBase::ProcessVariable_int16(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize)
{
  return ProcessIntVariable<int16_t>(rVariableInfo, buffer, bufferSize);
}

bool DataDispatcherBase::ProcessVariable_int32(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize)
{
  return ProcessIntVariable<int32_t>(rVariableInfo, buffer, bufferSize);
}

bool DataDispatcherBase::ProcessVariable_float(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize)
{
  return ProcessIntVariable<float>(rVariableInfo, buffer, bufferSize);
}

bool DataDispatcherBase::ProcessVariable_double(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize)
{
  return ProcessIntVariable<double>(rVariableInfo, buffer, bufferSize);
}

bool DataDispatcherBase::ProcessVariable_string(VariableData &rVariableInfo, uint8_t *buffer, uint8_t bufferSize)
{
  char *pValue = (char *)rVariableInfo.m_pVariable;

  uint8_t uMaxLength = rVariableInfo.m_uMaxBufferSize - 1;
  if (uMaxLength > bufferSize)
  {
    uMaxLength = bufferSize;
  }
  const char *pchSource = (char *)buffer;
  char *pchDestination = pValue;
  while (uMaxLength-- && *pchSource)
  {
    *pchDestination++ = *pchSource++;
  }
  *pchDestination = '\0';

  return true;
}
