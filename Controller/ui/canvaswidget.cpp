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
        return;

    m_image = image.convertToFormat(QImage::Format_Grayscale8).scaled(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation);

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

void CanvasWidget::toBitmap(uint8_t bitmap[SCREEN_HEIGHT][BYTES_PER_ROW])
{
    std::memset(bitmap, 0, sizeof(uint8_t) * SCREEN_HEIGHT * BYTES_PER_ROW);

    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (qGray(m_image.pixel(x, y)) < 128)
                bitmap[y][x / 8] |= (1 << (7 - (x % 8)));
        }
    }
}