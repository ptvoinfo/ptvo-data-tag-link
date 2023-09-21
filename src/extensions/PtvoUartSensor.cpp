#include "PtvoUartSensor.h"

using namespace PDTL;

void PtvoUartSensor<>::SendSysCommand(ESysCommand uCommandId)
{
   StreamParser::SendSysCommand((uint8_t)uCommandId);
}

void PtvoUartSensor<>::SendCommand(uint8_t Endpoint, EEndpointCommand uCommandId)
{
    StreamParser::SendCommand(Endpoint, (uint8_t)uCommandId);
}