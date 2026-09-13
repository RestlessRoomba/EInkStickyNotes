#pragma once

#include <cstdint>
#include <vector>


class SerialPort
{
    public:
        SerialPort();

        bool openSerial();

        bool sendData(const std::vector<std::uint8_t>& data);
        bool receiveData(std::vector<std::uint8_t>& data);

    private:
        bool waitForAck();
        void sendAck();
};
