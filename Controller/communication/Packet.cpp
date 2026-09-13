/*
--- Packet Architecture ---
Byte 0      MAGIC_1
Byte 1      MAGIC_2
Byte 2      TYPE
Byte 3 - 4  SIZE
Byte 5 - N  PAYLOAD
Byte N+1 - (N+1)+2  CRC16
*/

#include "Packet.h"


// Default Constructor Type = Ping
Packet::Packet() : m_type(MessageType::Ping)
{
}

// Constructor with Type and Payload
Packet::Packet(MessageType type, const QByteArray& payload) : m_type(type), m_payload(payload)
{
}

QByteArray Packet::serialize() const
{
    QByteArray data;

    // --- Header ---
    // Magic
    data.append(static_cast<char>(MAGIC_1));
    data.append(static_cast<char>(MAGIC_2));

    // Type
    data.append(static_cast<char>(m_type));

    // Payload Size
    uint16_t payloadSize = static_cast<uint16_t>(m_payload.size());
    data.append(static_cast<char>(payloadSize & 0xFF));
    data.append(static_cast<char>((payloadSize >> 8) & 0xFF));

    // --- Payload ---
    data.append(m_payload);

    return data;
}

Packet Packet::deserialize(const QByteArray& data)
{
    // Check for Min Size
    if (data.size() < 5)
    {
        throw std::invalid_argument("Packet too small.");
    }

    // Magic
    if (static_cast<uint8_t>(data[0]) != MAGIC_1 || static_cast<uint8_t>(data[1]) != MAGIC_2)
    {
        throw std::invalid_argument("invalid packet magic.");
    }

    // Type
    MessageType type = static_cast<MessageType>(static_cast<uint8_t>(data[2]));

    // Payload Size
    uint16_t payloadSize = static_cast<uint16_t>(static_cast<uint8_t>(data[3])) | (static_cast<uint16_t>(static_cast<uint8_t>(data[4])) << 8);

    // Check Payload Size
    if (data.size() != 5 + payloadSize)
    {
        throw std::invalid_argument("Invalid payload size");
    }

    QByteArray payload = data.mid(5, payloadSize);

    return Packet(type, payload);
}

MessageType Packet::type() const
{
    return m_type;
}

QByteArray Packet::payload() const
{
    return m_payload;
}
