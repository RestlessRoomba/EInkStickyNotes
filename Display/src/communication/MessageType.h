#pragma once

#include <cstdint>


enum class MessageType: uint8_t {
    Bitmap  = 0x01,
    Ack     = 0x02,
    Ping    = 0x03
};
