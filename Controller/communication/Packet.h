#pragma once

#include <QByteArray>
#include <cstdint>

#include "MessageType.h"


class Packet 
{
    public:
        Packet();
        Packet(MessageType type, const QByteArray& payload);

        QByteArray serialize() const;
        static Packet deserialize(const QByteArray& data);

        MessageType type() const;
        QByteArray payload() const;

    private:
        static constexpr uint8_t MAGIC_1 = 0xAA;
        static constexpr uint8_t MAGIC_2 = 0x55;

        MessageType m_type;
        QByteArray m_payload;
};
