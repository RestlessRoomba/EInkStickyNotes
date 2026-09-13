#pragma once

#include <cstdint>

constexpr int SCREEN_WIDTH = 400;
constexpr int SCREEN_HEIGHT = 300;
constexpr int BYTES_PER_ROW = SCREEN_WIDTH / 8;
constexpr int SCREEN_SIZE = SCREEN_HEIGHT * BYTES_PER_ROW;

extern uint8_t screen[SCREEN_HEIGHT][BYTES_PER_ROW];

void setPixel(int x, int y, bool value);
bool getPixel(int x, int y);
void fillBitmap();
int clearScreen();
int sendBitmap();
