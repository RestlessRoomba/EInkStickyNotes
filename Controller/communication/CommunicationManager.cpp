#include "CommunicationManager.h"
#include "Packet.h"
#include "payloads/BitmapPayload.h"


CommunicationManager::CommunicationManager()
{
}

bool CommunicationManager::open()
{
    return m_serialPort.openSerial();
}

bool CommunicationManager::sendBitmap(const QByteArray& bitmap)
{
    BitmapPayload payload(bitmap);
    Packet packet(MessageType::Bitmap, payload.serialize());
    QByteArray data = packet.serialize();

    return m_serialPort.sendData(data);
}
