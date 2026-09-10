#include "mainwindow.h"
#include "main.cpp"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  // Create the button, make "this" the parent
  m_button = new QPushButton("My Button", this);
  // set size and location of the button
  m_button->setGeometry(QRect(QPoint(100, 100), QSize(100, 100)));

  QPushButton *clearButton = new QPushButton("Clear Display", this);
  clearButton->setGeometry(QRect(QPoint(250, 100), QSize(100, 100)));

  // Connect button signal to appropriate slot
  connect(m_button, &QPushButton::released, this, &MainWindow::handleButton);
  connect(clearButton, &QPushButton::released, this, &MainWindow::handleClearButton);
}
 
void MainWindow::handleButton()
{
  sendBitmap(&::screen[0][0]);
}

void MainWindow::handleClearButton()
{
  // Clear the screen
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      setPixel(x, y, false);
    }
  }
  // Send the cleared bitmap
  sendBitmap(&::screen[0][0]);
}