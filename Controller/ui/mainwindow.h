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
  void handleButton();
  void handleClearButton();
  void handleImportButton();

private:
  void sendBitmap();

  SerialPort m_serialPort;
  CommunicationManager m_communicationManager;

  std::vector<uint8_t> m_screen;

  QPushButton *m_button;
  QPushButton *m_clearButton;
  CanvasWidget *m_canvas;
};
#endif // MAINWINDOW_H