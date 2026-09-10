#include "communication/SerialPort.h"

#include <QApplication>
#include "mainwindow.h"
#include <QDebug>
#include <QThread>
#include <QElapsedTimer>
#include <algorithm>


#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 300
#define BYTES_PER_ROW (SCREEN_WIDTH / 8)


void sendBitmap(uint8_t* bitmap);
void waitForAck();
void sendAck();
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

    SerialPort serialPort;
    serialPort.openSerial();

    // Send Bitmap
    sendBitmap(&screen[0][0]);

    return a.exec();
}

void sendBitmap(uint8_t* bitmap) {
    const char* data = reinterpret_cast<const char*>(bitmap);
    const int DATA_SIZE = SCREEN_HEIGHT * BYTES_PER_ROW;
    const int CHUNK_SIZE = 256;

    for (int bytesSent = 0; bytesSent < DATA_SIZE; bytesSent += CHUNK_SIZE) {
        int bytesToSend = std::min(CHUNK_SIZE, DATA_SIZE - bytesSent);
        Serial.write(data + bytesSent, bytesToSend);
        waitForAck();
    }
}

void waitForAck() {
    QByteArray ack;

    while (ack.size() < 3) {
        if (Serial.waitForReadyRead()) {
            ack += Serial.readAll();
        }
    }

    if (ack.left(3) == "ACK") {
        return;
    }
}

void sendAck() {
    Serial.write("ACK");
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
