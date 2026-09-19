#pragma once

#include "SerialPort.h"
#include "PacketParser.h"

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <functional>


class CommunicationManager
{
    public:
        using BitmapReceivedCallback = std::function<void(const std::vector<std::uint8_t>&)>;
        void setBitmapReceivedCallback(BitmapReceivedCallback callback);

        explicit CommunicationManager(SerialPort& serialPort);
        
        bool sendBitmap(const std::vector<std::uint8_t>& bitmap);

        bool getBitmap();

        void process();

        const std::vector<std::uint8_t>& currentBitmap() const;
        const std::vector<uint8_t>& receivedBitmap() const;

        void setCurrentBitmap(const std::vector<std::uint8_t>& bitmap);

    private:
        struct PendingRequest
        {
            MessageType expectedResponse;
        };

        std::uint16_t createRequestId();

        void completeRequest(std::uint16_t requestId);

        bool sendBitmap(const std::vector<std::uint8_t>& bitmap, std::uint16_t requestId);

    private:
        SerialPort& m_serialPort;
        PacketParser m_packetParser;

        std::vector<std::uint8_t> m_currentBitmap;

        std::uint16_t m_nextRequestId = 1;

        std::unordered_map<std::uint16_t, PendingRequest> m_pendingRequests;

        BitmapReceivedCallback m_bitmapReceivedCallback;
};
