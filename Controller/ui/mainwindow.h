#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <vector>
#include <cstdint>

#include "communication/SerialPort.h"
#include "canvaswidget.h"
#include "main.h"
#include "communication/CommunicationManager.h"

namespace Ui {
  class MainWindow;
}
 
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override = default;

private slots:
  void handleSendButton();
  void handleClearButton();
  void handleImportButton();
  void changeColor(const QColor &color, QPushButton *button, QPushButton *const allButtons[]);
private:
  void sendBitmap();
  void getBitmap();

  SerialPort m_serialPort;
  CommunicationManager m_communicationManager;

  QPushButton *m_sendButton;
  QPushButton *m_clearButton;
  CanvasWidget *m_canvas;
};
#endif // MAINWINDOW_H