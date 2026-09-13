#pragma once

#include <cstdint>
#include <vector>

#include "MessageType.h"


class Packet 
{
    public:
        Packet();
        Packet(MessageType type, const std::vector<std::uint8_t>& payload);

        std::vector<std::uint8_t> serialize() const;
        static Packet deserialize(const std::vector<std::uint8_t>& data);

        MessageType type() const;
        std::vector<std::uint8_t> payload() const;

    private:
        static constexpr std::uint8_t MAGIC_1 = 0xAA;
        static constexpr std::uint8_t MAGIC_2 = 0x55;

        MessageType m_type;
        std::vector<std::uint8_t> m_payload;
};
