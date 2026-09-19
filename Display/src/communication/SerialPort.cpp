#include "SerialPort.h"

#include <Arduino.h>
#include <algorithm>
#include <utility>


namespace
{
    constexpr std::uint8_t MAGIC_1 = 0xAA;
    constexpr std::uint8_t MAGIC_2 = 0x55;

    constexpr char ACK_1 = 'A';
    constexpr char ACK_2 = 'C';
    constexpr char ACK_3 = 'K';
}

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
    if (data.empty())
        return true;

    if (m_txActive)
        return false;

    m_txData = data;
    m_txOffset = 0;
    m_txChunkSize = 0;
    m_txChunkSent = 0;

    m_txActive = true;
    m_waitingForAck = false;

    startNextTransmitChunk();

    return true;
}

void SerialPort::startNextTransmitChunk()
{
    if (!m_txActive)
        return;

    if (m_waitingForAck)
        return;

    if (m_txOffset >= m_txData.size())
    {
        m_txData.clear();

        m_txOffset = 0;
        m_txChunkSize = 0;
        m_txChunkSent = 0;

        m_txActive = false;

        return;
    }

    const std::size_t remaining =
        m_txData.size() - m_txOffset;

    m_txChunkSize =
        std::min(CHUNK_SIZE, remaining);

    m_txChunkSent = 0;

    processTransmit();
}

void SerialPort::processTransmit()
{
    if (!m_txActive)
        return;

    if (m_waitingForAck)
        return;

    if (m_txChunkSent >= m_txChunkSize)
    {
        m_txOffset += m_txChunkSize;

        m_waitingForAck = true;

        return;
    }

    const std::size_t remaining =
        m_txChunkSize - m_txChunkSent;

    const int available =
        Serial.availableForWrite();

    if (available <= 0)
        return;

    const std::size_t bytesToWrite =
        std::min(
            remaining,
            static_cast<std::size_t>(available)
        );

    const std::size_t written =
        Serial.write(
            m_txData.data()
                + m_txOffset
                + m_txChunkSent,
            bytesToWrite
        );

    m_txChunkSent += written;

    if (m_txChunkSent >= m_txChunkSize)
    {
        m_txOffset += m_txChunkSize;
        m_waitingForAck = true;
    }
}

void SerialPort::handleTransmitAck(std::uint8_t byte)
{
    static std::uint8_t ackState = 0;

    switch (ackState)
    {
        case 0:
            if (byte == ACK_1)
                ackState = 1;
            break;

        case 1:
            if (byte == ACK_2)
                ackState = 2;
            else if (byte == ACK_1)
                ackState = 1;
            else
                ackState = 0;
            break;

        case 2:
            if (byte == ACK_3)
            {
                ackState = 0;

                m_waitingForAck = false;

                if (m_txOffset >= m_txData.size())
                {
                    m_txData.clear();

                    m_txOffset = 0;
                    m_txChunkSize = 0;
                    m_txChunkSent = 0;

                    m_txActive = false;
                }
                else
                {
                    startNextTransmitChunk();
                }
            }
            else if (byte == ACK_1)
            {
                ackState = 1;
            }
            else
            {
                ackState = 0;
            }

            break;
    }
}

void SerialPort::processReceive()
{
    while (Serial.available() > 0)
    {
        const std::uint8_t byte =
            static_cast<std::uint8_t>(
                Serial.read()
            );

        if (m_waitingForAck)
        {
            handleTransmitAck(byte);
            continue;
        }

        m_rxPacketData.push_back(byte);
    }

    processReceivedData();
}

void SerialPort::processReceivedData()
{
    if (m_rxExpectedSize == 0)
    {
        if (m_rxPacketData.size() < HEADER_SIZE)
            return;

        if (m_rxPacketData[0] != MAGIC_1 ||
            m_rxPacketData[1] != MAGIC_2)
        {
            m_rxPacketData.erase(
                m_rxPacketData.begin()
            );

            return;
        }

        const std::uint16_t payloadSize =
            static_cast<std::uint16_t>(
                m_rxPacketData[3]
            ) |
            (
                static_cast<std::uint16_t>(
                    m_rxPacketData[4]
                ) << 8
            );

        m_rxExpectedSize =
            HEADER_SIZE + payloadSize;
    }

    while (true)
    {
        const std::size_t remaining =
            m_rxExpectedSize - m_rxAckedBytes;

        const std::size_t expectedChunkSize =
            std::min(
                CHUNK_SIZE,
                remaining
            );

        if (m_rxPacketData.size() <
            m_rxAckedBytes + expectedChunkSize)
        {
            return;
        }

        m_receivedData.insert(
            m_receivedData.end(),
            m_rxPacketData.begin()
                + m_rxAckedBytes,
            m_rxPacketData.begin()
                + m_rxAckedBytes
                + expectedChunkSize
        );

        m_rxAckedBytes += expectedChunkSize;

        sendTransportAck();

        if (m_rxAckedBytes >= m_rxExpectedSize)
        {
            m_rxPacketData.clear();

            m_rxAckedBytes = 0;
            m_rxExpectedSize = 0;

            if (m_dataAvailableCallback)
            {
                m_dataAvailableCallback();
            }

            return;
        }
    }
}

void SerialPort::sendTransportAck()
{
    Serial.write("ACK", 3);
}


std::vector<std::uint8_t> SerialPort::readAvailable()
{
    std::vector<std::uint8_t> data = std::move(m_receivedData);
    m_receivedData.clear();

    return data;
}

void SerialPort::setDataAvailableCallback(DataAvailableCallback callback)
{
    m_dataAvailableCallback = std::move(callback);
}

void SerialPort::process()
{
    processTransmit();
    processReceive();
}