#include "communication/CommunicationManager.h"
#include "mainwindow.h"
#include "canvaswidget.h"

#include <QFileDialog>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
  m_canvas = new CanvasWidget(this);
  m_canvas->setGeometry(20, 45, 128, 64);

  // ToolBox Buttons
  QHBoxLayout *toolbox = new QHBoxLayout(this);
  toolbox->addWidget(m_canvas);

  QPushButton *penBtn = new QPushButton("P", this);
  toolbox->addWidget(penBtn);
  penBtn->setGeometry(25, 10, 30, 30);

  QPushButton *eraserBtn = new QPushButton("E", this);
  toolbox->addWidget(eraserBtn);
  eraserBtn->setGeometry(60, 10, 30, 30);

  QPushButton *textBtn = new QPushButton("T", this);
  toolbox->addWidget(textBtn);
  textBtn->setGeometry(95, 10, 30, 30);

  QPushButton *lineBtn = new QPushButton("L", this);
  toolbox->addWidget(lineBtn);
  lineBtn->setGeometry(130, 10, 30, 30);

  QPushButton *rectBtn = new QPushButton("R", this);
  toolbox->addWidget(rectBtn);  
  rectBtn->setGeometry(165, 10, 30, 30);

  QPushButton *circleBtn = new QPushButton("C", this);
  toolbox->addWidget(circleBtn);
  circleBtn->setGeometry(200, 10, 30, 30);

  QPushButton *fillBtn = new QPushButton("F", this);
  toolbox->addWidget(fillBtn);
  fillBtn->setGeometry(235, 10, 30, 30);

  QPushButton *gColorBtn = new QPushButton("G", this);
  toolbox->addWidget(gColorBtn);
  gColorBtn->setGeometry(270, 10, 30, 30);

  QPushButton *dgColorBtn = new QPushButton("DG", this);
  toolbox->addWidget(dgColorBtn);
  dgColorBtn->setGeometry(305, 10, 30, 30);

  QPushButton *lgColorBtn = new QPushButton("LG", this);
  toolbox->addWidget(lgColorBtn);
  lgColorBtn->setGeometry(340, 10, 30, 30);

  QPushButton *wColorBtn = new QPushButton("W", this);
  toolbox->addWidget(wColorBtn);
  wColorBtn->setGeometry(375, 10, 30, 30);

  QPushButton *undoBtn = new QPushButton("Undo", this);
  toolbox->addWidget(undoBtn);
  undoBtn->setGeometry(410, 10, 60, 30);
  // toolBox end


  QPushButton *importButton =
      new QPushButton("Import Image", this);
  importButton->setGeometry(500, 20, 120, 40);

  QPushButton *clearButton =
      new QPushButton("Clear Display", this);
  clearButton->setGeometry(500, 70, 120, 40);

  QPushButton *sendButton =
      new QPushButton("Send Bitmap", this);
  sendButton->setGeometry(500, 120, 120, 40);

  connect(importButton, &QPushButton::clicked, this, &MainWindow::handleImportButton);
  connect(clearButton, &QPushButton::clicked, this, &MainWindow::handleClearButton);
  connect(sendButton, &QPushButton::clicked, this, &MainWindow::handleSendButton);
  connect(penBtn, &QPushButton::clicked, m_canvas, [this]() { m_canvas->setTool('P'); });
  connect(textBtn, &QPushButton::clicked, m_canvas, [this]() { m_canvas->setTool('T'); });
  connect(lineBtn, &QPushButton::clicked, m_canvas, [this]() { m_canvas->setTool('L'); });
  connect(rectBtn, &QPushButton::clicked, m_canvas, [this]() { m_canvas->setTool('R'); });
  connect(circleBtn, &QPushButton::clicked, m_canvas, [this]() { m_canvas->setTool('C'); });
  connect(fillBtn, &QPushButton::clicked, m_canvas, [this]() { m_canvas->setTool('F'); });
  connect(eraserBtn, &QPushButton::clicked, m_canvas, [this]() { m_canvas->setColor(Qt::white); });
  connect(gColorBtn, &QPushButton::clicked, m_canvas, [this]() { m_canvas->setColor(Qt::gray); });
}

void MainWindow::handleSendButton()
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