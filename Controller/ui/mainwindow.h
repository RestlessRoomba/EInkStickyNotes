#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

#include <cstdint>

#include "communication/SerialPort.h"

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 300
#define BYTES_PER_ROW (SCREEN_WIDTH / 8)

extern uint8_t screen[SCREEN_HEIGHT][BYTES_PER_ROW];
extern SerialPort serialPort;

void sendBitmap(uint8_t* bitmap);
void setPixel(int x, int y, bool value);
bool getPixel(int x, int y);

namespace Ui {
  class MainWindow;
}
 
class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);
private slots:
  void handleButton();
  void handleClearButton();
private:
  QPushButton *m_button;
  QPushButton *m_clearButton;
};
#endif // MAINWINDOW_H