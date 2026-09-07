#include <QApplication>
#include "mainwindow.h"
#include <iostream>
#include <QSerialPort>
#include <QDebug>
#include <QThread>
#include <QElapsedTimer>

const QString ESP32_PORT = "/dev/serial/by-id/usb-Espressif_USB_JTAG_serial_debug_unit_94:A9:90:DD:A6:4C-if00";

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 300
#define BYTES_PER_ROW (SCREEN_WIDTH / 8)


void setPixel(int x, int y, bool value);
bool getPixel(int x, int y);

uint8_t screen[SCREEN_HEIGHT][BYTES_PER_ROW];

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // Fill Bitmap
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            setPixel(x, y, x % 2);
        }
    }
    

    // Open Serial
    QSerialPort serial;
    serial.setPortName(ESP32_PORT);
    serial.setBaudRate(QSerialPort::Baud115200);

    if (!serial.open(QIODevice::ReadWrite)) {
        qDebug() << "Error while opening serial: " << serial.errorString();
        return 1;
    }

    qDebug() << "Serial opened.";

    // Send Bitmap-Message
    const char* data = reinterpret_cast<const char*>(screen);
    const int data_size = sizeof(data);
    const int chunk_size = 256;

    for (int i = 0; i < data_size; i += chunk_size) {
        int bytesToSend = min(chunk_size, data_size - i);
        Serial.write(data + i, bytesToSend);
        // Hier muss die schleife auf ein ACK vom esp warten
    }

    return a.exec();
}

void setPixel(int x, int y, bool value) {
    if (value) {
        screen[y][x / 8] |= (1 << (7 - (x % 8)));
    } else {
        screen[y][x / 8] &= ~(1 << (7 - (x % 8)));
        }
    }

bool getPixel(int x, int y) {
        return (screen[y][x / 8] >> (7 - (x % 8))) & 1;
    }
