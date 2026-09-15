#include "communication/SerialPort.h"
#include "communication/Packet.h"
#include "communication/MessageType.h"
#include "communication/payloads/BitmapPayload.h"

#include <vector>

#include <Arduino.h>
#include <SPI.h>
#include <GxEPD2_4G_4G.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>


// ---------- Pin-Connections ESP32-S3-DevKitC-1 ----------
#define EPD_CS    10
#define EPD_DC    17
#define EPD_RST   18
#define EPD_BUSY  16

#define EPD_SCK   12
#define EPD_MOSI  11
#define EPD_MISO  13

// ---------- Colors ----------
#define BLACK GxEPD_BLACK
#define WHITE GxEPD_WHITE
#define DARKGRAY GxEPD_DARKGREY
#define LIGHTGRAY GxEPD_LIGHTGREY

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 300

constexpr int BITS_PER_PIXEL = 2;
constexpr int PIXELS_PER_BYTE = 4;
constexpr int BYTES_PER_ROW = SCREEN_WIDTH / PIXELS_PER_BYTE;
constexpr int SCREEN_SIZE = SCREEN_HEIGHT * BYTES_PER_ROW;


// ---------- Display-Driver (V2) ----------
GxEPD2_4G_4G<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(GxEPD2_420_GDEY042T81(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));


// ---------- Code ----------

SerialPort serialPort;

void drawScreen(const std::vector<std::uint8_t>& bitmap);
void receiveBitmap();


void setup()
{
  serialPort.openSerial();

  SPI.begin(EPD_SCK, EPD_MISO, EPD_MOSI, EPD_CS);
  display.init(115200, true, 2, false);
  display.setFullWindow();

  display.firstPage();

  do {
    display.fillScreen(WHITE);
  } while (display.nextPage());
}

void loop()
{
  receiveBitmap();
}

void receiveBitmap()
{
  std::vector<uint8_t> data;

  if (!serialPort.receiveData(data))
  {
    return;
  }

  Packet packet = Packet::deserialize(data);

  if (packet.type() != MessageType::Bitmap)
  {
    return;
  }

  BitmapPayload payload = BitmapPayload::deserialize(packet.payload());

  drawScreen(payload.bitmap());
}

void drawScreen(const std::vector<std::uint8_t>& bitmap)
{
  if (bitmap.size() != SCREEN_SIZE)
  {
    return;
  }

  display.drawImage_4G(
      bitmap.data(),
      2,              // 2 bits per pixel
      0,
      0,
      SCREEN_WIDTH,
      SCREEN_HEIGHT
  );
}
