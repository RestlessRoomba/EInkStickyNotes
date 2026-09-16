#include "communication/CommunicationManager.h"
#include "mainwindow.h"
#include "canvaswidget.h"
#include "tool.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
  this->setWindowTitle("E-Ink Sticky Notes");

  m_canvas = new CanvasWidget(this);
        m_canvas->setFixedSize(128 * 4, 64 * 4); // Scale up the canvas for better visibility

  // ToolBox ====================================
  QHBoxLayout *toolbox = new QHBoxLayout();
  toolbox->setSpacing(5);
  toolbox->setContentsMargins(0, 0, 0, 0);

  QPushButton *bColorBtn = new QPushButton("B", this);
  bColorBtn->setFixedSize(30, 30);
  bColorBtn->setCheckable(true);
  toolbox->addWidget(bColorBtn);

  QPushButton *dgColorBtn = new QPushButton("DG", this);
  dgColorBtn->setCheckable(true);
  dgColorBtn->setFixedSize(30, 30);
  toolbox->addWidget(dgColorBtn);

  QPushButton *lgColorBtn = new QPushButton("LG", this);
  lgColorBtn->setFixedSize(30, 30);
  lgColorBtn->setCheckable(true);
  toolbox->addWidget(lgColorBtn);

  QPushButton *wColorBtn = new QPushButton("W", this);
  wColorBtn->setFixedSize(30, 30);
  wColorBtn->setCheckable(true);
  toolbox->addWidget(wColorBtn);

  QPushButton *allColors[] = {bColorBtn, dgColorBtn, lgColorBtn, wColorBtn};

  QPushButton *textBtn = new QPushButton("T", this);
  textBtn->setFixedSize(30, 30);
  toolbox->addWidget(textBtn);

  QPushButton *penBtn = new QPushButton("P", this);
  penBtn->setFixedSize(30, 30);
  toolbox->addWidget(penBtn);

  QPushButton *lineBtn = new QPushButton("L", this);
  lineBtn->setFixedSize(30, 30);
  toolbox->addWidget(lineBtn);

  QPushButton *rectBtn = new QPushButton("R", this);
  rectBtn->setFixedSize(30, 30);
  toolbox->addWidget(rectBtn);

  QPushButton *circleBtn = new QPushButton("C", this);
  circleBtn->setFixedSize(30, 30);
  toolbox->addWidget(circleBtn);

  QPushButton *clearBtn = new QPushButton("Clear", this);
  clearBtn->setFixedSize(30, 30);
  toolbox->addWidget(clearBtn);

  toolbox->addStretch(); // pushes everything to the left


  QSlider *sizeSlider = new QSlider(Qt::Horizontal, this);
  sizeSlider->setRange(1, 10);
  sizeSlider->setValue(1);

  QVBoxLayout *canvasLayout = new QVBoxLayout();
  canvasLayout->addLayout(toolbox);
  canvasLayout->addWidget(m_canvas);
  canvasLayout->addWidget(sizeSlider);

  // Control Buttons ==============================

  QVBoxLayout *controlBtnsLayout = new QVBoxLayout();

  QPushButton *importButton = new QPushButton("Import Image", this);
  importButton->setFixedSize(120, 40);
  controlBtnsLayout->addWidget(importButton);

  QPushButton *clearButton = new QPushButton("Clear Display", this);
  clearButton->setFixedSize(120, 40);
  controlBtnsLayout->addWidget(clearButton);

  QPushButton *sendButton = new QPushButton("Send Bitmap", this);
  sendButton->setFixedSize(120, 40);
  controlBtnsLayout->addWidget(sendButton);

  controlBtnsLayout->addStretch();

  // Main Layout ==============================

  QHBoxLayout *mainLayout = new QHBoxLayout();
  mainLayout->addLayout(canvasLayout);
  mainLayout->addLayout(controlBtnsLayout);

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setLayout(mainLayout);

  setCentralWidget(centralWidget);

  connect(importButton, &QPushButton::clicked, this, &MainWindow::handleImportButton);
  connect(clearButton, &QPushButton::clicked, this, &MainWindow::handleClearButton);
  connect(sendButton, &QPushButton::clicked, this, &MainWindow::handleSendButton);
  connect(textBtn, &QPushButton::clicked, m_canvas, [this](){ changeTool(Tool::Text); });
  connect(lineBtn, &QPushButton::clicked, m_canvas, [this](){ changeTool(Tool::Line); });
  connect(rectBtn, &QPushButton::clicked, m_canvas, [this](){ changeTool(Tool::Rectangle); });
  connect(circleBtn, &QPushButton::clicked, m_canvas, [this](){ changeTool(Tool::Circle); });
  connect(penBtn, &QPushButton::clicked, m_canvas, [this](){ changeTool(Tool::Pen); });
  connect(wColorBtn, &QPushButton::clicked, m_canvas, [this, wColorBtn, allColors](){ changeColor(QColor(WHITE, WHITE, WHITE), wColorBtn, allColors); });
  connect(bColorBtn, &QPushButton::clicked, m_canvas, [this, bColorBtn, allColors](){ changeColor(QColor(BLACK, BLACK, BLACK), bColorBtn, allColors); });
  connect(dgColorBtn, &QPushButton::clicked, m_canvas, [this, dgColorBtn, allColors](){ changeColor(QColor(DARKGRAY, DARKGRAY, DARKGRAY), dgColorBtn, allColors); });
  connect(lgColorBtn, &QPushButton::clicked, m_canvas, [this, lgColorBtn, allColors](){ changeColor(QColor(LIGHTGRAY, LIGHTGRAY, LIGHTGRAY), lgColorBtn, allColors); });
  connect(clearBtn, &QPushButton::clicked, m_canvas, &CanvasWidget::clear);
  connect(sizeSlider, &QSlider::valueChanged, m_canvas, &CanvasWidget::setToolSize);
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

void MainWindow::changeColor(const QColor &color, QPushButton *button, QPushButton *const allButtons[])
{
  m_canvas->setColor(color);

  for (int index = 0; index < 4; ++index)
  {
    if (allButtons[index] != button)
    {
      allButtons[index]->setChecked(false);
      allButtons[index]->setCheckable(true);
    }
  }
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