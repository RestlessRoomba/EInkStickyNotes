#include "SerialPort.h"

#include <QObject>
#include <QDebug>
#include <algorithm>
#include <utility>


const QString ESP32_PORT = "/dev/serial/by-id/usb-Espressif_USB_JTAG_serial_debug_unit_94:A9:90:DD:A6:4C-if00";

SerialPort::SerialPort()
{
    QObject::connect(&m_serial, &QSerialPort::readyRead, [this](){handleReadyRead();});
}

bool SerialPort::openSerial()
{
    m_serial.setPortName(ESP32_PORT);
    m_serial.setBaudRate(QSerialPort::Baud115200);

    if (!m_serial.open(QIODevice::ReadWrite)) {
        qDebug() << "Error while opening serial: " << m_serial.errorString();
        return false;
    }

    m_serial.clear(QSerialPort::AllDirections); // Clear Serial

    qDebug() << "Serial opened.";
    return true;
}

bool SerialPort::isOpen() const
{
    return m_serial.isOpen();
}

bool SerialPort::sendData(const std::vector<std::uint8_t>& data)
{
    if (data.empty())
    {
        return true;
    }

    // Only one transfer at a time (WIP)
    if (m_txActive)
    {
        return false;
    }

    m_txData = data;
    m_txOffset = 0;
    m_txActive = true;  // Accept new Transfer
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
        m_txActive = false;
        return;
    }

    const std::size_t remaining = m_txData.size() - m_txOffset;
    const std::size_t chunkSize = std::min(CHUNK_SIZE, remaining);
    const qint64 written = m_serial.write(reinterpret_cast<const char*>(m_txData.data() + m_txOffset), static_cast<qint64>(chunkSize));

    if (written != static_cast<qint64>(chunkSize))
    {
        qDebug() << "Failed to queue complete serial chunk.";
        m_txActive = false;

        return;
    }

    m_txOffset += chunkSize;
    m_waitingForAck = true;
}

void SerialPort::handleReadyRead()
{
    QByteArray data = m_serial.readAll();

    for (char byte : data)
    {
        const std::uint8_t value = static_cast<std::uint8_t>(static_cast<unsigned char>(byte));

        if (m_waitingForAck)
        {
            m_ackBuffer.push_back(value);

            if (m_ackBuffer.size() >= 3)
            {
                const std::size_t size = m_ackBuffer.size();

                if (m_ackBuffer[size - 3] == 'A' &&
                    m_ackBuffer[size - 2] == 'C' &&
                    m_ackBuffer[size - 1] == 'K')
                {
                    m_ackBuffer.clear();
                    m_waitingForAck = false;

                    if (m_txOffset >= m_txData.size())
                    {
                        m_txData.clear();
                        m_txOffset = 0;
                        m_txActive = false;
                    }
                    else 
                    {
                        startNextTransmitChunk();
                    }

                    continue;
                }
            }

            continue;
        }

        // Not waiting for ACK -> Data belongs to Packet Data
        m_rxPacketData.push_back(value);
    }

    processReceivedData();
}

void SerialPort::processReceivedData()
{
    constexpr std::uint8_t MAGIC_1 = 0xAA;
    constexpr std::uint8_t MAGIC_2 = 0x55;

    // Not enough bytes for header
    if (m_rxExpectedSize == 0)
    {
        if (m_rxPacketData.size() < HEADER_SIZE)
            return;

        // Check Magic
        if (m_rxPacketData[0] != MAGIC_1 || m_rxPacketData[1] != MAGIC_2)
        {
            m_rxPacketData.erase(m_rxPacketData.begin());
            return;
        }

        // Get Payload Size
        const std::uint16_t payloadSize = static_cast<std::uint16_t>(m_rxPacketData[3]) | (static_cast<uint16_t>(m_rxPacketData[4]) << 8);
        m_rxExpectedSize = HEADER_SIZE + payloadSize;
    }

    // Check, how many bytes are left to receive until ACK is required
    while (m_rxAckedBytes < m_rxExpectedSize && m_rxPacketData.size() >= m_rxAckedBytes + std::min(CHUNK_SIZE, m_rxExpectedSize - m_rxAckedBytes))
    {
        const std::size_t remaining = m_rxExpectedSize - m_rxAckedBytes;
        const std::size_t chunkSize = std::min(CHUNK_SIZE, remaining);

        // Pass this block
        m_receivedData.insert(m_receivedData.end(), m_rxPacketData.begin() + m_rxAckedBytes, m_rxPacketData.begin() + m_rxAckedBytes + chunkSize);

        m_rxAckedBytes += chunkSize;

        // Send Trasnport Ack
        sendTransportAck();

        // Packet complete?
        if (m_rxAckedBytes >= m_rxExpectedSize)
        {
            // Packet complete!
            m_rxPacketData.clear();
            m_rxAckedBytes = 0;
            m_rxExpectedSize = 0;

            if (m_dataAvailableCallback)
            {
                m_dataAvailableCallback();
            }

            break;
        }
    }
}

bool SerialPort::sendTransportAck()
{
    const qint64 written = m_serial.write("ACK", 3);

    return written == 3;
}

std::vector<uint8_t> SerialPort::readAvailable()
{
    std::vector<uint8_t> data = std::move(m_receivedData);
    m_receivedData.clear();

    return data;
}

void SerialPort::setDataAvailableCallback(DataAvailableCallback callback)
{
    m_dataAvailableCallback = std::move(callback);
}
