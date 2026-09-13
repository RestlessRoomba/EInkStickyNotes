#pragma once

#include "SerialPort.h"

#include <cstdint>
#include <vector>


class CommunicationManager
{
    public:
        explicit CommunicationManager(SerialPort& serialPort);
        
        bool sendBitmap(const std::vector<std::uint8_t>& bitmap);

    private:
        SerialPort& m_serialPort;
};
