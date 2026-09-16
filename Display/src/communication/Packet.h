/*
--- Packet Architecture ---
Byte 0              MAGIC_1
Byte 1              MAGIC_2
Byte 2              TYPE
Byte 3-4            PAYLOAD SIZE
Byte 5-6            REQUEST ID
Byte 7-N            PAYLOAD
*/

#pragma once

#include <cstdint>
#include <vector>

#include "MessageType.h"


class Packet 
{
    public:
        Packet();
        Packet(MessageType type, std::uint16_t requestId, const std::vector<std::uint8_t>& payload);

        std::vector<std::uint8_t> serialize() const;
        static Packet deserialize(const std::vector<std::uint8_t>& data);

        MessageType type() const;
        std::uint16_t requestId() const;
        const std::vector<std::uint8_t>& payload() const;

    private:
        static constexpr std::uint8_t MAGIC_1 = 0xAA;
        static constexpr std::uint8_t MAGIC_2 = 0x55;

        MessageType m_type;
        std::uint16_t m_requestId;
        std::vector<std::uint8_t> m_payload;
};
