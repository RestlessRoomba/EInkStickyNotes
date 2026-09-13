#include "BitmapPayload.h"


BitmapPayload::BitmapPayload(const QByteArray& bitmap) : m_bitmap(bitmap)
{
}

QByteArray BitmapPayload::serialize() const
{
    return m_bitmap;
}

BitmapPayload BitmapPayload::deserialize(const QByteArray& data)
{
    return BitmapPayload(data);
}

QByteArray BitmapPayload::bitmap() const
{
    return m_bitmap;
}
