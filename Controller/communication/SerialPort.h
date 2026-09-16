#pragma once

#include <cstdint>
#include <vector>
#include <functional>

#include <QSerialPort>


class SerialPort
{
    public:
        using DataAvailableCallback = std::function<void()>;

        SerialPort();

        bool openSerial();

        bool sendData(const std::vector<std::uint8_t>& data);

        std::vector<std::uint8_t> readAvailable();

        void setDataAvailableCallback(DataAvailableCallback callback);

    private:
        static constexpr std::size_t CHUNK_SIZE = 256;
        static constexpr std::size_t HEADER_SIZE = 7;

        void startNextTransmitChunk();
        void handleReadyRead();
        void processReceivedData();
        bool sendTransportAck();

    private:
        QSerialPort m_serial;

        DataAvailableCallback m_dataAvailableCallback;

        // TX State
        std::vector<std::uint8_t> m_txData;
        std::size_t m_txOffset = 0;
        bool m_txActive = false;
        bool m_waitingForAck = false;

        // RX State
        std::vector<std::uint8_t> m_rxPacketData;
        std::size_t m_rxAckedBytes = 0;
        std::size_t m_rxExpectedSize = 0;

        std::vector<std::uint8_t> m_receivedData;
        std::vector<std::uint8_t> m_ackBuffer;
};
