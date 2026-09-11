#include "SerialPort.h"

#include <QDebug>
#include <algorithm>


const QString ESP32_PORT = "/dev/serial/by-id/usb-Espressif_USB_JTAG_serial_debug_unit_94:A9:90:DD:A6:4C-if00";

SerialPort::SerialPort()
{
}

bool SerialPort::openSerial()
{
    m_serial.setPortName(ESP32_PORT);
    m_serial.setBaudRate(QSerialPort::Baud115200);

    if (!m_serial.open(QIODevice::ReadWrite)) {
        qDebug() << "Error while opening serial: " << m_serial.errorString();
        return false;
    }

    m_serial.clear(QSerialPort::AllDirections); // Clear Serial

    qDebug() << "Serial opened.";
    return true;
}

bool SerialPort::sendData(const QByteArray& data)
{
    constexpr qsizetype CHUNK_SIZE = 256;

    for (int bytes = 0; bytes < data.size(); bytes += CHUNK_SIZE)
    {
        int bytesToSend = std::min(CHUNK_SIZE, data.size() - bytes);
        QByteArray chunk = data.mid(bytes, bytesToSend);
        qDebug() << "Sending" << bytesToSend << "bytes at offset" << bytes;
        qint64 written = m_serial.write(chunk);

        if (written != bytesToSend)
        {
            qDebug() << "Error: Not all bytes were accepted by QSerialPort.";
            return false;
        }

        if (!m_serial.waitForBytesWritten(1000))
        {
            qDebug() << "Error while writing to serial: " << m_serial.errorString();
            return false;
        }

        if (!waitForAck())
        {
            qDebug() << "Error: No valid ACK received.";
            return false;
        }
    }
    return true;
}

bool SerialPort::waitForAck()
{
    constexpr int TIMEOUT = 1000;
    QByteArray received;

    while (true)
    {
        if (!m_serial.waitForReadyRead(TIMEOUT))
        {
            qDebug() << "Timeout while waiting for ACK.";
            return false;
        }

        received.append(m_serial.readAll());

        if (received.contains("ACK"))
        {
            qDebug() << "ACK received.";
            return true;
        }
    }
}
