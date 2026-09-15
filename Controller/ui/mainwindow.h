#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

#include "communication/SerialPort.h"
#include "canvaswidget.h"
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
  void handleSendButton();
  void handleClearButton();
  void handleImportButton();
  void changeColor(const QColor &color, QPushButton *button, QPushButton *const allButtons[]);
private:
  QPushButton *m_sendButton;
  QPushButton *m_clearButton;
  CanvasWidget *m_canvas;
};
#endif // MAINWINDOW_H