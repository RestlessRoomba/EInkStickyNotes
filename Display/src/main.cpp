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


// ---------- Functions ----------
void drawScreen(uint8_t* bitmap);
bool receiveBitmap(uint8_t* bitmap);
void waitForAck();
void sendAck();


uint8_t bitmap[SCREEN_SIZE];


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
}

void loop() {
  receiveBitmap(bitmap);
  drawScreen(bitmap);
}

void drawScreen(uint8_t* bitmap) {
  display.firstPage();

  do {
    display.fillScreen(WHITE);
    display.drawBitmap(
      0,
      0,
      bitmap,
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
      BLACK
    );
  } while (display.nextPage());
}

bool receiveBitmap(uint8_t* bitmap) {
  size_t received = 0;
  const size_t CHUNK_SIZE = 256;

  while (received < SCREEN_SIZE) {
    size_t bytesToReceive = min(CHUNK_SIZE, SCREEN_SIZE - received);

    while (Serial.available() < bytesToReceive) {
      // wait
    }

    for (size_t bytes = 0; bytes < bytesToReceive; bytes++) {
      bitmap[received++] = Serial.read();
    }
    
    sendAck();
  }

  return true;
}

void waitForAck() {
    bool ackReceived = false;

    while (ackReceived == false) {
        if (Serial.available()) {
            if (Serial.read() == 'A' &&
                Serial.read() == 'C' &&
                Serial.read() == 'K') {
                    ackReceived = true;
                }
        }
    }
}

void sendAck() {
    Serial.write("ACK");
}
