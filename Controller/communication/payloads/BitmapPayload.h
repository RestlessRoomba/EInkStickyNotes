#pragma once

#include <QByteArray>


class BitmapPayload
{
    public:
        BitmapPayload(const QByteArray& bitmap);

        QByteArray serialize() const;
        static BitmapPayload deserialize(const QByteArray& data);

        QByteArray bitmap() const;

    private:
        QByteArray m_bitmap;
};
