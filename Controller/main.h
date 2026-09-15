#pragma once

#include <cstdint>
#include <vector>

constexpr int SCREEN_WIDTH = 400;
constexpr int SCREEN_HEIGHT = 300;

constexpr int BITS_PER_PIXEL = 2;
constexpr int PIXELS_PER_BYTE = 4;
constexpr int BYTES_PER_ROW = SCREEN_WIDTH / PIXELS_PER_BYTE;
constexpr int SCREEN_SIZE = SCREEN_HEIGHT * BYTES_PER_ROW;

// 4 Grayscale Values of the Display
constexpr int BLACK     = 0;
constexpr int DARKGRAY  = 128;
constexpr int LIGHTGRAY = 192;
constexpr int WHITE     = 255;

inline std::vector<std::uint8_t> screen(SCREEN_SIZE, WHITE);

int clearScreen();
int sendBitmap();
