#include <Arduino.h>
#include <SPI.h>
#include <GxEPD2_BW.h>
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

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 300

#define BYTES_PER_ROW (SCREEN_WIDTH / 8)
#define SCREEN_SIZE (SCREEN_HEIGHT * BYTES_PER_ROW)


// ---------- Display-Driver (V2) ----------
GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(
    GxEPD2_420_GDEY042T81(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));


uint8_t screen[SCREEN_SIZE];


// ---------- Functions ----------
void drawScreen() {
  display.firstPage();

  do {
    display.fillScreen(WHITE);
    display.drawBitmap(
      0,
      0,
      (uint8_t*)screen,
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
      BLACK
    );
  } while (display.nextPage());
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("\nStarted.");

  SPI.begin(EPD_SCK, EPD_MISO, EPD_MOSI, EPD_CS);
  display.init(115200, true, 2, false);
  display.setFullWindow();

  display.firstPage();

  do {
    display.fillScreen(WHITE);
  } while (display.nextPage());

  Serial.println("READY");
}

void loop() {
  if (Serial.available()) {
    size_t received = 0;

    for (int i = 0; i < SCREEN_SIZE / 256; i++) {
      for (int j = 0; j < 256; j++) {
        screen[received] = Serial.read();
      }
      Serial.write("ACK\n");
    }
  }
}
