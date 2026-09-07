#include <QApplication>
#include "mainwindow.h"
#include <iostream>

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 300
#define BYTES_PER_ROW (SCREEN_WIDTH / 8)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    // Create Test Array
    uint8_t screen[SCREEN_HEIGHT][BYTES_PER_ROW];

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

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            setPixel(x, y, x % 2);
        }
    }

    return a.exec();
}
