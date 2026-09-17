#include "Packet.h"

#include <stdexcept>

const std::uint8_t Packet::MAGIC_1 = 0xAA;
const std::uint8_t Packet::MAGIC_2 = 0x55;

// Default Constructor Type = Ping
Packet::Packet() : m_type(MessageType::Ping), m_requestId(0)
{
}

// Constructor with Type and Payload
Packet::Packet(MessageType type, std::uint16_t requestId, const std::vector<std::uint8_t>& payload) : m_type(type), m_requestId(requestId), m_payload(payload)
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

    // Request ID
    data.push_back(static_cast<std::uint8_t>(m_requestId & 0xFF));
    data.push_back(static_cast<uint8_t>((m_requestId >> 8) & 0xFF));

    // --- Payload ---
    data.insert(data.end(), m_payload.begin(), m_payload.end());

    return data;
}

Packet Packet::deserialize(const std::vector<std::uint8_t>& data)
{
    // Check for Min Size
    // MAGIC1 + MAGIC2 + TYPE + SIZE(2) + REQ ID(2) = 7 Bytes
    constexpr std::size_t HEADER_SIZE = 7;

    if (data.size() < HEADER_SIZE)
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

    // Request ID
    std::uint16_t requestId = static_cast<uint16_t>(data[5]) | (static_cast<uint16_t>(data[6]) << 8);

    // Check total Package Size
    if (data.size() != HEADER_SIZE + payloadSize)
    {
        throw std::invalid_argument("Invalid packet size");
    }

    // Extract Payload
    std::vector<std::uint8_t> payload(data.begin() + HEADER_SIZE, data.end());

    return Packet(type, requestId, payload);
}

MessageType Packet::type() const
{
    return m_type;
}

std::uint16_t Packet::requestId() const
{
    return m_requestId;
}

const std::vector<std::uint8_t>& Packet::payload() const
{
    return m_payload;
}
