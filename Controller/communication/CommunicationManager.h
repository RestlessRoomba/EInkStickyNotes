#pragma once

#include "SerialPort.h"

#include <QByteArray>


class CommunicationManager
{
    public:
        CommunicationManager();

        bool open();
        
        bool sendBitmap(const QByteArray& bitmap);

    private:
        SerialPort m_serialPort;
};
