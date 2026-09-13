#include "CommunicationManager.h"
#include "Packet.h"
#include "payloads/BitmapPayload.h"


CommunicationManager::CommunicationManager(SerialPort& SerialPort) : m_serialPort(SerialPort)
{
}

bool CommunicationManager::sendBitmap(const std::vector<std::uint8_t>& bitmap)
{
    BitmapPayload payload(bitmap);
    Packet packet(MessageType::Bitmap, payload.serialize());
    std::vector<std::uint8_t> data = packet.serialize();

    return m_serialPort.sendData(data);
}
