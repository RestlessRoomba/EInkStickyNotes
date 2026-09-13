#include "SerialPort.h"

#include <Arduino.h>
#include <algorithm>


SerialPort::SerialPort()
{
}

bool SerialPort::openSerial()
{
    Serial.begin(115200);

    return true;
}

bool SerialPort::sendData(const std::vector<std::uint8_t>& data)
{
    constexpr std::size_t CHUNK_SIZE = 256;

    for (std::size_t bytes = 0; bytes < data.size(); bytes += CHUNK_SIZE)
    {
        std::size_t bytesToSend = std::min(CHUNK_SIZE, data.size() - bytes);

        Serial.write(data.data() + bytes, bytesToSend);
        Serial.flush();

        if (!waitForAck())
        {
            return false;
        }
    }
    
    return true;
}

bool SerialPort::receiveData(std::vector<std::uint8_t>& data)
{
    constexpr std::size_t CHUNK_SIZE = 256;
    constexpr std::size_t HEADER_SIZE = 5;
    constexpr unsigned long TIMEOUT = 1000;

    data.clear();

    // Receive Header
    unsigned long startTime = millis();

    while (data.size() < HEADER_SIZE)
    {
        while (Serial.available() > 0 && data.size() < HEADER_SIZE)
        {
            data.push_back(static_cast<std::uint8_t>(Serial.read()));
        }

        if (millis() - startTime >= TIMEOUT)
        {
            return false;
        }
    }

    // Check Header
    constexpr std::uint8_t MAGIC_1 = 0xAA;
    constexpr std::uint8_t MAGIC_2 = 0x55;

    if (data[0] != MAGIC_1 || data[1] != MAGIC_2)
    {
        return false;
    }

    // Read Payload Size
    std::uint16_t payloadSize = static_cast<std::uint16_t>(data[3]) | (static_cast<uint16_t>(data[4]) << 8);

    // Total Size
    std::size_t totalSize = HEADER_SIZE + payloadSize;

    // Receive Remaining First Block Data
    std::size_t firstChunkSize = std::min(CHUNK_SIZE, totalSize);

    startTime = millis();

    while (data.size() < firstChunkSize)
    {
        while (Serial.available() > 0 && data.size() < firstChunkSize)
        {
            data.push_back(static_cast<std::uint8_t>(Serial.read()));
        }

        if (millis() - startTime >= TIMEOUT)
        {
            return false;
        }
    }

    // Acknowledge First Block
    sendAck();

    // Receive Remaining Blocks
    while (data.size() < totalSize)
    {
        std::size_t remaining = totalSize - data.size();
        std::size_t chunkSize = std::min(CHUNK_SIZE, remaining);
        std::size_t targetSize = data.size() + chunkSize;

        startTime = millis();

        while (data.size() < targetSize)
        {
            while (Serial.available() > 0 && data.size() < targetSize)
            {
                data.push_back(static_cast<std::uint8_t>(Serial.read()));
            }

            if (millis() - startTime >= TIMEOUT)
            {
                return false;
            }
        }

        sendAck();
    }

    return true;
}

bool SerialPort::waitForAck()
{
    constexpr unsigned long TIMEOUT = 1000;
    unsigned long startTime = millis();

    std::vector<std::uint8_t> received;

    while (millis() - startTime < TIMEOUT)
    {
        while (Serial.available() > 0)
        {
            received.push_back(static_cast<std::uint8_t>(Serial.read()));

            if (received.size() >= 3)
            {
                std::size_t size = received.size();

                if (received[size - 3] == 'A' &&
                    received[size - 2] == 'C' &&
                    received[size - 1] == 'K')
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void SerialPort::sendAck()
{
    Serial.write("ACK");
}
