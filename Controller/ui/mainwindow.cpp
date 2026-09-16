#include "communication/CommunicationManager.h"
#include "mainwindow.h"
#include "canvaswidget.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_serialPort(), m_communicationManager(m_serialPort)
{
  m_serialPort.setDataAvailableCallback([this](){m_communicationManager.process();});
  m_serialPort.openSerial();

  m_canvas = new CanvasWidget(this);
  m_canvas->setGeometry(20, 20, 128, 64);

  QPushButton *importButton =
      new QPushButton("Import Image", this);
  importButton->setGeometry(500, 20, 120, 40);

  QPushButton *clearButton =
      new QPushButton("Clear Display", this);
  clearButton->setGeometry(500, 70, 120, 40);

  QPushButton *sendButton =
      new QPushButton("Send Bitmap", this);
  sendButton->setGeometry(500, 120, 120, 40);

  connect(importButton, &QPushButton::clicked,
          this, &MainWindow::handleImportButton);

  connect(clearButton, &QPushButton::clicked,
          this, &MainWindow::handleClearButton);

  connect(sendButton, &QPushButton::clicked,
          this, &MainWindow::handleButton);
}

void MainWindow::handleButton()
{
  m_canvas->toBitmap(::screen);
  ::sendBitmap();
}

void MainWindow::handleClearButton()
{
  ::clearScreen();
}

void MainWindow::handleImportButton()
{
  QString filename = QFileDialog::getOpenFileName(
      this,
      "Open Image",
      "",
      "Images (*.png *.jpg *.jpeg *.bmp)");

  if (!filename.isEmpty())
  {
    m_canvas->loadImage(filename);
  }
}