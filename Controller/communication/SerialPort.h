#pragma once

#include <QSerialPort>
#include <QByteArray>


class SerialPort
{
    public:
        SerialPort();

        bool openSerial();
        bool sendData(const QByteArray& data);

    private:
        bool waitForAck();

        QSerialPort m_serial;
};
