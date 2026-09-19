#pragma once

#include "Packet.h"

#include <cstdint>
#include <deque>
#include <vector>


class PacketParser
{
    public:
        void feed(const std::vector<std::uint8_t>& data);

        bool hasPacket() const;

        Packet takePacket();

    private:
        void parse();

        static constexpr std::uint8_t MAGIC_1 = 0xAA;
        static constexpr std::uint8_t MAGIC_2 = 0x55;

        static constexpr std::size_t HEADER_SIZE = 7;

        std::vector<std::uint8_t> m_buffer;
        std::deque<Packet> m_packets;
};