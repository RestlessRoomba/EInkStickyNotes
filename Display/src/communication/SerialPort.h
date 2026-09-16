#pragma once

#include <cstdint>
#include <functional>
#include <vector>
#include <cstddef>


class SerialPort
{
    public:
        using DataAvailableCallback = std::function<void()>;

        SerialPort();

        bool openSerial();

        bool sendData(const std::vector<std::uint8_t>& data);
        bool receiveData(std::vector<std::uint8_t>& data);

    private:
        bool waitForAck();
        void sendAck();
};
