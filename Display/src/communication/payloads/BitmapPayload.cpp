#include "BitmapPayload.h"


BitmapPayload::BitmapPayload(const std::vector<std::uint8_t>& bitmap) : m_bitmap(bitmap)
{
}

std::vector<std::uint8_t> BitmapPayload::serialize() const
{
    return m_bitmap;
}

BitmapPayload BitmapPayload::deserialize(const std::vector<std::uint8_t>& data)
{
    return BitmapPayload(data);
}

const std::vector<std::uint8_t> BitmapPayload::bitmap() const
{
    return m_bitmap;
}
