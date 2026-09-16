#include "CommunicationManager.h"
#include "Packet.h"
#include "payloads/BitmapPayload.h"


CommunicationManager::CommunicationManager(SerialPort& SerialPort) : m_serialPort(SerialPort)
{
}

const std::vector<std::uint8_t>& CommunicationManager::currentBitmap() const
{
    return m_currentBitmap;
}

void CommunicationManager::setCurrentBitmap(const std::vector<std::uint8_t>& bitmap)
{
    m_currentBitmap = bitmap;
}

// Public: Normal Bitmap
bool CommunicationManager::sendBitmap(const std::vector<uint8_t>& bitmap)
{
    return sendBitmap(bitmap, 0);
}

// Private: Bitmap-Answer-Message
bool CommunicationManager::sendBitmap(const std::vector<std::uint8_t>& bitmap, std::uint16_t requestId)
{
    BitmapPayload payload(bitmap);
    Packet packet(MessageType::Bitmap, requestId, payload.serialize());
    std::vector<std::uint8_t> data = packet.serialize();

    return m_serialPort.sendData(data);
}

bool CommunicationManager::getBitmap()
{
    // Create and register request ID
    const std::uint16_t requestId = createRequestId();
    m_pendingRequests.emplace(requestId, PendingRequest{MessageType::Bitmap});

    Packet packet(MessageType::GetBitmap, requestId, {});
    std::vector<uint8_t> data = packet.serialize();

    if (!m_serialPort.sendData(data))
    {
        completeRequest(requestId);
        return false;
    }

    return true;
}

void CommunicationManager::process()
{
    std::vector<std::uint8_t> data = m_serialPort.readAvailable();

    if (data.empty())
    {
        return;
    }

    // Pass bytes into Packet Parser
    m_packetParser.feed(data);

    // Process complete Packets
    while (m_packetParser.hasPacket())
    {
        Packet packet = m_packetParser.takePacket();

        switch (packet.type())
        {
            case MessageType::Bitmap:
            {
                const std::uint16_t requestId = packet.requestId();

                // Normal BitmapMessage without Request
                if (requestId == 0)
                {
                    BitmapPayload payload = BitmapPayload::deserialize(packet.payload());
                    m_currentBitmap = payload.bitmap();

                    break;
                }

                // Check for valid pending Request
                auto it = m_pendingRequests.find(requestId);

                if (it == m_pendingRequests.end())
                {
                    // No valid pending request
                    break;
                }

                // Check if this request awaits a BitmapMessage
                if (it->second.expectedResponse != MessageType::Bitmap)
                {
                    // Wrong answer MessageType
                    break;
                }

                BitmapPayload payload = BitmapPayload::deserialize(packet.payload());
                m_currentBitmap = payload.bitmap();

                // Request is complete
                completeRequest(requestId);
                break;
            }

            case MessageType::GetBitmap:
            {
                const std::uint16_t requestId = packet.requestId();

                if (requestId == 0)
                {
                    // GetBitmap without Request-ID is invalid
                    break;
                }

                sendBitmap(m_currentBitmap, requestId);

                break;
            }

            default:
            {
                // Unknown MessageType
                break;
            }
        }
    }
}

void CommunicationManager::completeRequest(std::uint16_t requestId)
{
    if (requestId == 0)
    {
        return;
    }

    m_pendingRequests.erase(requestId);
}