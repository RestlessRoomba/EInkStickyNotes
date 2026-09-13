/*
--- Packet Architecture ---
Byte 0              MAGIC_1
Byte 1              MAGIC_2
Byte 2              TYPE
Byte 3 - 4          SIZE
Byte 5 - N          PAYLOAD
Byte N+1 - (N+1)+2  CRC16       ---> not implemented yet
*/

#include "Packet.h"

#include <stdexcept>


// Default Constructor Type = Ping
Packet::Packet() : m_type(MessageType::Ping)
{
}

// Constructor with Type and Payload
Packet::Packet(MessageType type, const std::vector<std::uint8_t>& payload) : m_type(type), m_payload(payload)
{
}

std::vector<std::uint8_t> Packet::serialize() const
{
    std::vector<std::uint8_t> data;

    // --- Header ---
    // Magic
    data.push_back(MAGIC_1);
    data.push_back(MAGIC_2);

    // Type
    data.push_back(static_cast<std::uint8_t>(m_type));

    // Payload Size
    std::uint16_t payloadSize = static_cast<std::uint16_t>(m_payload.size());
    data.push_back(static_cast<std::uint8_t>(payloadSize & 0xFF));
    data.push_back(static_cast<std::uint8_t>((payloadSize >> 8) & 0xFF));

    // --- Payload ---
    data.insert(data.end(), m_payload.begin(), m_payload.end());

    return data;
}

Packet Packet::deserialize(const std::vector<std::uint8_t>& data)
{
    // Check for Min Size
    // MAGIC1 + MAGIC2 + TYPE + SIZE(2) = 5 Bytes
    if (data.size() < 5)
    {
        throw std::invalid_argument("Packet too small.");
    }

    // Magic
    if (data[0] != MAGIC_1 || data[1] != MAGIC_2)
    {
        throw std::invalid_argument("Invalid packet magic.");
    }

    // Type
    MessageType type = static_cast<MessageType>(data[2]);

    // Payload Size
    std::uint16_t payloadSize = static_cast<std::uint16_t>(data[3]) | (static_cast<std::uint16_t>(data[4]) << 8);

    // Check total Package Size
    if (data.size() != 5 + payloadSize)
    {
        throw std::invalid_argument("Invalid payload size");
    }

    std::vector<std::uint8_t> payload(data.begin() + 5, data.begin() + 5 + payloadSize);

    return Packet(type, payload);
}

MessageType Packet::type() const
{
    return m_type;
}

std::vector<std::uint8_t> Packet::payload() const
{
    return m_payload;
}
