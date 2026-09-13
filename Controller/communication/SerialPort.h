#pragma once

#include <cstdint>
#include <vector>

#include <QSerialPort>


class SerialPort
{
    public:
        SerialPort();

        bool openSerial();
        bool sendData(const std::vector<std::uint8_t>& data);

    private:
        bool waitForAck();

        QSerialPort m_serial;
};
