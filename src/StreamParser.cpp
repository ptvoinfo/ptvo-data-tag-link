#include "StreamParser.h"

using namespace PDTL;

StreamParser::StreamParser(PDTL::DataDispatcherBase &rCommandHandler,
                           uint8_t *pchReceiveBuffer,
                           unsigned uBufferSize,
                           Stream &rSourceStream /*= Serial*/,
                           uint8_t chStartOfMessage /*= '!'*/,
                           uint8_t chEndOfMessage /*= ';'*/)
    : m_pCommandHandler(&rCommandHandler), m_pSource(&rSourceStream), m_chStartOfMessage(chStartOfMessage), m_chEndOfMessage(chEndOfMessage), m_pchBuffer(pchReceiveBuffer), m_uMaxBufferSize(uBufferSize)
{
  Reset();
}

StreamParser::StreamParser(uint8_t *pchReceiveBuffer, unsigned uBufferSize)
    : m_pchBuffer(pchReceiveBuffer), m_uMaxBufferSize(uBufferSize)
{
  Reset();
}

// calculate a checksum of incoming data as a simple arithmetic sum
uint8_t StreamParser::crc8_sum(uint8_t *buf, uint8_t len)
{
  uint8_t crc = 0x00;

  for (uint8_t i = 0; i < len; i++)
  {
    crc += buf[i];
  }
  return crc;
}

// process all bytes from the serial buffer
void StreamParser::Process()
{
  uint8_t ch;
  uint8_t chPrev;

  // ignore incomplete (damaged or bad) data packets
  if ((millis() - uLastByteReceived) > 5000)
  {
    Reset();
  }

  while (m_pSource != NULL && m_pSource->available() > 0)
  {
    ch = m_pSource->read();

    if (ch == m_chStartOfMessage)
    {
      Reset();
      m_pchBuffer[0] = ch; // the start character serves as a flag that this message is valid.
      ++m_uNextCharacter;
      continue;
    }

    // Store the character and advance the buffer. If there isn't enough
    // room for the message will be discarded
    chPrev = m_pchBuffer[m_uNextCharacter - 1];
    m_pchBuffer[m_uNextCharacter++] = ch;

    if (m_uNextCharacter >= (m_uMaxBufferSize - 1))
    {
      Reset();
    }
    else if (chPrev == m_chEndOfMessage)
    {
      if (m_uNextCharacter > 0 && m_pchBuffer[0] == m_chStartOfMessage)
      {
        uint8_t size = m_uNextCharacter;
        uint8_t crc1 = crc8_sum(m_pchBuffer, size - 1); // w/o CRC
        uint8_t crc2 = m_pchBuffer[m_uNextCharacter - 1];

        if (crc1 == crc2)
        {
          // We have a valid message. Dispatch it.
          size = UnescapeData(&m_pchBuffer[0], size);
          DispatchMessage(&m_pchBuffer[1], size - 3);
        }
        Reset();
      }
    }
  }

  uLastByteReceived = millis();
}

// without start and end signatures, crc, all data unescaped
void StreamParser::DispatchMessage(uint8_t *pData, uint8_t size)
{
  if (size < 2)
  {
    return;
  }

  uint8_t data_type = (*pData & PDTL_DATA_TYPE_MASK) >> PDTL_DATA_TYPE_SHIFT;
  bool is_command = ((data_type == PDTL_DATA_TYPE_COMMAND) || (data_type == PDTL_DATA_TYPE_SYS_COMMAND));
  uint8_t uId = *pData & PDTL_ID_MASK;
  pData++;
  size--;

  if (!is_command)
  {
    m_pCommandHandler->DispatchVariable(uId, *m_pSource, pData, size);
    return;
  }

  uint8_t uCmd = *pData;
  if ((data_type == PDTL_DATA_TYPE_COMMAND) && (uCmd == PDTL_COMMAND_GET_DATA))
  {
    VariableData *pVariable = m_pCommandHandler->FindVariable(uId);
    if (pVariable != NULL)
    {
      if (pVariable->m_fnCallback)
      {
        // call to prepare data if needed
        pVariable->m_fnCallback(*pVariable, false);
      }
      SendVariable(uId);
      return;
    }
  }

  m_pCommandHandler->DispatchCommand(uCmd, uId, *m_pSource);
}

void StreamParser::Reset()
{
  m_uNextCharacter = 0;
}

// escape special bytes in outgoing data
// returns the number of escaped bytes
uint8_t StreamParser::EscapeData(uint8_t *src, uint8_t *dest, uint8_t size)
{
  uint8_t ch;
  uint8_t res = 0;

  for (uint8_t i = 0; i < size; i++)
  {
    ch = *src;

    if (ch == m_chStartOfMessage)
    {
      *dest = STREAM_ESC_SIGN;
      dest++;
      res++;
      ch = STREAM_ESCAPED_START;
    }
    else if (ch == m_chEndOfMessage)
    {
      *dest = STREAM_ESC_SIGN;
      dest++;
      res++;
      ch = STREAM_ESCAPED_END;
    }
    else if (ch == STREAM_ESC_SIGN)
    {
      *dest = STREAM_ESC_SIGN;
      dest++;
      res++;
      ch = STREAM_ESCAPED_ESC;
    }

    *dest = ch;
    dest++;
    src++;
    res++;
  }

  return res;
}

// unescape all incoming data in place
// returns the number of unescaped bytes
uint8_t StreamParser::UnescapeData(uint8_t *src, uint8_t size)
{
  uint8_t ch;
  uint8_t chPrev = 0;
  uint8_t res = 0;
  uint8_t *dest = src;

  for (uint8_t i = 0; i < size; i++)
  {
    ch = src[i];

    if (chPrev == STREAM_ESC_SIGN)
    {
      if (ch == STREAM_ESCAPED_START)
      {
        res--;
        dest[res] = (uint8_t)m_chStartOfMessage;
      }
      else if (ch == STREAM_ESCAPED_END)
      {
        res--;
        dest[res] = (uint8_t)m_chEndOfMessage;
      }
      else if (ch == STREAM_ESCAPED_ESC)
      {
        res--;
        dest[res] = STREAM_ESC_SIGN;
      }
    }
    else
    {
      dest[res] = ch;
    }

    res++;
    chPrev = ch;
  }

  return res;
}

void StreamParser::SendByteCommand(const uint8_t uVariableId, bool command, uint8_t uValue)
{
  uint8_t buffer[3 + sizeof(uValue) * 2];
  buffer[0] = m_chStartOfMessage;
  buffer[1] = uVariableId & PDTL_ID_MASK;
  uint8_t data_type = PDTL_DATA_TYPE_UINT8;

  if (command)
  {
    data_type = (uVariableId == PDTL_ID_SYS) ? PDTL_DATA_TYPE_SYS_COMMAND : PDTL_DATA_TYPE_COMMAND;
  }

  buffer[1] |= (data_type << PDTL_DATA_TYPE_SHIFT);
  uint8_t len = 2;
  len += EscapeData(&uValue, &buffer[2], 1);
  buffer[len] = m_chEndOfMessage;
  len++;
  buffer[len] = crc8_sum(&buffer[0], len);
  len++;
  m_pSource->write(buffer, len);
}

void StreamParser::SendCommand(const uint8_t uVariableId, const uint8_t Command)
{
  SendByteCommand(uVariableId, true, Command);
}

void StreamParser::SendSysCommand(const uint8_t Command)
{
  SendByteCommand(PDTL_ID_SYS, true, Command);
}

void StreamParser::SendValue(const uint8_t uVariableId, uint8_t *pValue, uint8_t uSize, uint8_t uDatatype)
{
  // allocate buffer to escape every byte in value
  uint8_t buffer[3 + uSize * 2];
  buffer[0] = m_chStartOfMessage;
  buffer[1] = (uVariableId & PDTL_ID_MASK) | (uDatatype << PDTL_DATA_TYPE_SHIFT);
  uint8_t len = 2;
  len += EscapeData(pValue, &buffer[2], uSize);
  buffer[len] = m_chEndOfMessage;
  len++;
  buffer[len] = crc8_sum(&buffer[0], len);
  len++;
  m_pSource->write(buffer, len);
}

void StreamParser::SendFloat(const uint8_t uVariableId, float fValue)
{
  SendValue(uVariableId, (uint8_t *)&fValue, sizeof(fValue), PDTL_DATA_TYPE_FLOAT);
}

void StreamParser::SendUInt8(const uint8_t uVariableId, uint8_t uValue)
{
  SendValue(uVariableId, (uint8_t *)&uValue, sizeof(uValue), PDTL_DATA_TYPE_UINT8);
}

void StreamParser::SendUInt16(const uint8_t uVariableId, uint16_t uValue)
{
  SendValue(uVariableId, (uint8_t *)&uValue, sizeof(uValue), PDTL_DATA_TYPE_UINT16);
}

void StreamParser::SendUInt32(const uint8_t uVariableId, uint32_t uValue)
{
  SendValue(uVariableId, (uint8_t *)&uValue, sizeof(uValue), PDTL_DATA_TYPE_UINT32);
}

void StreamParser::SendInt8(const uint8_t uVariableId, int8_t uValue)
{
  SendValue(uVariableId, (uint8_t *)&uValue, sizeof(uValue), PDTL_DATA_TYPE_INT8);
}

void StreamParser::SendInt16(const uint8_t uVariableId, int16_t uValue)
{
  SendValue(uVariableId, (uint8_t *)&uValue, sizeof(uValue), PDTL_DATA_TYPE_INT16);
}

void StreamParser::SendInt32(const uint8_t uVariableId, int32_t uValue)
{
  SendValue(uVariableId, (uint8_t *)&uValue, sizeof(uValue), PDTL_DATA_TYPE_INT32);
}

void StreamParser::SendVariable(const uint8_t uVariableId)
{
  VariableData *pVarData = m_pCommandHandler->FindVariable(uVariableId);
  if ((pVarData == NULL) || (pVarData->m_pVariable == NULL))
  {
    return;
  }
  if (pVarData->m_uMaxBufferSize > 0)
  {
    SendValue(uVariableId, (uint8_t *)pVarData->m_pVariable, pVarData->m_uMaxBufferSize, pVarData->m_uDatatype);
  }
}
