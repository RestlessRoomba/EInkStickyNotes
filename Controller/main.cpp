#include "communication/CommunicationManager.h"
#include "ui/mainwindow.h"
#include "communication/SerialPort.h"

#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QElapsedTimer>
#include <algorithm>
#include <vector>


#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 300
#define BYTES_PER_ROW (SCREEN_WIDTH / 8)
#define SCREEN_SIZE (SCREEN_HEIGHT * BYTES_PER_ROW)


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

    // --- Send a Bitmap via CommunicationManager ---
    // Open a Serial Port
    SerialPort serialPort;

    if (!serialPort.openSerial())
    {
        qDebug() << "Error while opening serial port.";
        return 1;
    }

    // Create a Communication Manager on that Serial Port
    CommunicationManager communication(serialPort);

    // Type conversion (uint8_t 2D Array into std::vector<std::uint8_t>)
    std::vector<std::uint8_t> bitmap;
    bitmap.reserve(SCREEN_SIZE);

    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < BYTES_PER_ROW; ++x) {
            bitmap.push_back(screen[y][x]);
        }
    }

    if (!communication.sendBitmap(bitmap))
    {
        qDebug() << "Error while sending bitmap.";
        return 1;
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
