#pragma once

#include <QSerialPort>


class SerialPort {
    public:
        SerialPort();
        bool openSerial();

    private:
        QSerialPort m_serial;
};
