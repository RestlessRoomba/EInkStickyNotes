#include "PacketParser.h"

#include <stdexcept>
#include <utility>


void PacketParser::feed(const std::vector<std::uint8_t>& data)
{
    m_buffer.insert(m_buffer.end(), data.begin(), data.end());
    parse();
}

bool PacketParser::hasPacket() const
{
    return !m_packets.empty();
}

Packet PacketParser::takePacket()
{
    if (m_packets.empty())
    {
        throw std::runtime_error("No packet available.");
    }

    Packet packet = std::move(m_packets.front());

    m_packets.pop_front();

    return packet;
}

void PacketParser::parse()
{
    while (true)
    {
        // Enough Data for Magic?
        if (m_buffer.size() < 2)
        {
            return;
        }

        // Check Magic
        if (m_buffer[0] != MAGIC_1 || m_buffer[1] != MAGIC_2)
        {
            m_buffer.erase(m_buffer.begin());

            continue;
        }

        // Wait for complete header to arrive
        if (m_buffer.size() < HEADER_SIZE)
        {
            return;
        }

        // Read Payload Size
        std::uint16_t payloadSize = static_cast<std::uint16_t>(m_buffer[3]) | (static_cast<std::uint16_t>(m_buffer[4]) << 8);

        const std::size_t totalSize = HEADER_SIZE + payloadSize;

        // Wait for complete packet to arrive
        if (m_buffer.size() < totalSize)
        {
            return;
        }

        // Extract Packet
        std::vector<uint8_t> packetData(m_buffer.begin(), m_buffer.begin() + totalSize);

        m_buffer.erase(m_buffer.begin(), m_buffer.begin() + totalSize);

        // Deserialize Packet & Push to stack
        Packet packet = Packet::deserialize(packetData);
        m_packets.push_back(std::move(packet));
    }
}