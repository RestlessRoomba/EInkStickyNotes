#include "SerialPort.h"

#include <QDebug>


const QString ESP32_PORT = "/dev/serial/by-id/usb-Espressif_USB_JTAG_serial_debug_unit_94:A9:90:DD:A6:4C-if00";

SerialPort::SerialPort() {

}

bool SerialPort::openSerial() {
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
