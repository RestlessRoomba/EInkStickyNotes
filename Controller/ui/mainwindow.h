#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

#include "communication/SerialPort.h"
#include "main.h"
extern SerialPort serialPort;

void sendBitmap(uint8_t* bitmap);

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