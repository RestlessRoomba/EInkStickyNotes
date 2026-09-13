#include "communication/CommunicationManager.h"
#include "main.h"
#include "ui/mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QElapsedTimer>
#include <algorithm>


uint8_t screen[SCREEN_HEIGHT][BYTES_PER_ROW];


int main(int argc, char *argv[]) 
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    fillBitmap();

    sendBitmap();

    return a.exec();
}

void fillBitmap() 
{
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            setPixel(x, y, x % 2);
        }
    }
}

int sendBitmap() 
{
    // --- Send a Bitmap via CommunicationManager ---
    CommunicationManager communication;

    if (!communication.open())
    {
        qDebug() << "Error while opening serial port.";
        return 1;
    }

    // Type conversion (uint8_t 2D Array into QByteArray)
    QByteArray bitmap(reinterpret_cast<const char*>(&screen[0][0]), SCREEN_HEIGHT * BYTES_PER_ROW);

    if (!communication.sendBitmap(bitmap))
    {
        qDebug() << "Error while sending bitmap.";
        return 1;
    }

    return 0;
}

int clearScreen() 
{
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            setPixel(x, y, false);
        }
    }

    CommunicationManager communication;
    if (!communication.open())
    {
        qDebug() << "Error while opening serial port.";
        return 1;
    }

    // Type conversion (uint8_t 2D Array into QByteArray)
    QByteArray bitmap(reinterpret_cast<const char*>(&screen[0][0]), SCREEN_HEIGHT * BYTES_PER_ROW);

    if (!communication.sendBitmap(bitmap))
    {
        qDebug() << "Error while sending bitmap.";
        return 1;
    }

    return 0;
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
