#include "communication/CommunicationManager.h"
#include "mainwindow.h"
#include "canvaswidget.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_serialPort(), m_communicationManager(m_serialPort)
{
  m_serialPort.setDataAvailableCallback([this](){m_communicationManager.process(); });
  m_communicationManager.setBitmapReceivedCallback(
    [this](const std::vector<uint8_t>& bitmap)
    {
      m_communicationManager.setCurrentBitmap(bitmap);
      qDebug() << "Bitmap received: " << bitmap.size() << " bytes";
      qDebug() << "Set received bitmap as current bitmap.";
    }
  );

  if (!m_serialPort.openSerial())
  {
    qDebug() << "Error while opening serial port.";
  }

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
  std::vector<uint8_t> bitmap;
  m_canvas->toBitmap(bitmap);
  m_communicationManager.setCurrentBitmap(bitmap);
  sendBitmap();
}

void MainWindow::handleClearButton()
{
  m_canvas->clear();

  std::vector<uint8_t> cleared(SCREEN_SIZE, WHITE);
  m_communicationManager.setCurrentBitmap(cleared);
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

void MainWindow::sendBitmap()
{
  if (!m_serialPort.isOpen())
  {
    if (!m_serialPort.openSerial())
    {
      qDebug() << "Error while opening serial port.";
      return;
    }
  }

  if (!m_communicationManager.sendBitmap(m_communicationManager.currentBitmap()))
  {
    qDebug() << "Could not start bitmap transfer.";
  }
}

void MainWindow::getBitmap()
{
  if (!m_serialPort.isOpen())
  {
    if (!m_serialPort.openSerial())
    {
      qDebug() << "Error while opening serial port.";
      return;
    }
  }

  if (!m_communicationManager.getBitmap())
  {
    qDebug() << "Could not start getBitmap transfer.";
  }
}