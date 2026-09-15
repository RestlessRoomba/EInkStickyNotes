#include "canvaswidget.h"

#include <QPainter>
#include <QPen>
#include <QMouseEvent>

#include <cstring>

CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent),
      m_image(SCREEN_WIDTH,
              SCREEN_HEIGHT,
              QImage::Format_Grayscale8)
{
    m_image.fill(Qt::white);

    setFixedSize(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void CanvasWidget::loadImage(const QString &filename)
{
    QImage image(filename);

    if (image.isNull())
    {
        return;
    }

    // Convert to the 4 allowed gray-values
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            const int gray = qGray(image.pixel(x, y));

            int quantizedGray;

            if (gray < 64)
            {
                quantizedGray = BLACK;
            } 
            else if (gray < 160)
            {
                quantizedGray = DARKGRAY;
            }
            else if (gray < 224)
            {
                quantizedGray = LIGHTGRAY;
            }
            else 
            {
                quantizedGray = WHITE;
            }

            image.setPixel(x, y, qRgb(quantizedGray, quantizedGray, quantizedGray));
        }
    }

    m_image = image;

    update();
}

void CanvasWidget::clear()
{
    m_image.fill(Qt::white);
    update();
}

void CanvasWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, m_image);
}

void CanvasWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_drawing = true;
        m_lastPoint = event->position().toPoint();
    }
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_drawing)
        return;

    const QPoint point = event->position().toPoint();
    QPainter painter(&m_image);
    painter.setPen(QPen(currentColor, currentSize));
    painter.drawLine(m_lastPoint, point);
    m_lastPoint = point;
    update();
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_drawing = false;
}

void CanvasWidget::setTool(char tool)
{
    m_currentTool = tool;
}

void CanvasWidget::setColor(const QColor &color)
{
    currentColor = color;
}

void CanvasWidget::setToolSize(int size)
{
    currentSize = size;
}

void CanvasWidget::toBitmap(std::vector<std::uint8_t>& bitmap)
{
    bitmap.assign(SCREEN_SIZE, 0);

    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            const int gray = qGray(m_image.pixel(x, y));  // Convert Color to Gray (0 to 255)

            std::uint8_t pixelValue;

            if (gray == BLACK)
            {
                pixelValue = 0;
            }
            else if (gray == DARKGRAY)
            {
                pixelValue = 1;
            }
            else if (gray == LIGHTGRAY)
            {
                pixelValue = 2;
            }
            else // WHITE
            {
                pixelValue = 3;
            }

            const int byteIndex = y * BYTES_PER_ROW + (x / 4);

            const int shift = 6 - 2 * (x % 4);

            bitmap[byteIndex] |= static_cast<std::uint8_t>(pixelValue << shift);
        }
    }
}