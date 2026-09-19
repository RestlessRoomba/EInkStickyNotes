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

        std::vector<std::uint8_t> readAvailable();

        void setDataAvailableCallback(DataAvailableCallback callback);

        void process();

    private:
        static constexpr std::size_t CHUNK_SIZE = 256;
        static constexpr std::size_t HEADER_SIZE = 7;

        // TX
        void processTransmit();
        void startNextTransmitChunk();
        void handleTransmitAck(std::uint8_t byte);

        // RX
        void processReceive();
        void processReceivedData();
        void sendTransportAck();

    private:
        DataAvailableCallback m_dataAvailableCallback;

        // TX State
        std::vector<uint8_t> m_txData;
        std::size_t m_txOffset = 0;
        std::size_t m_txChunkSize = 0;
        std::size_t m_txChunkSent = 0;
        
        bool m_txActive = false;
        bool m_waitingForAck = false;

        // RX State
        std::vector<std::uint8_t> m_rxPacketData;
        std::size_t m_rxAckedBytes = 0;
        std::size_t m_rxExpectedSize = 0;
        std::vector<std::uint8_t> m_receivedData;
};
