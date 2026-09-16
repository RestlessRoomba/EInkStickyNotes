#include "canvaswidget.h"
#include "tool.h"

#include <QPainter>
#include <QPen>
#include <QMouseEvent>

#include <cstring>

CanvasWidget::CanvasWidget(QWidget *parent) : QWidget(parent), tempImage(SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format_ARGB32_Premultiplied), image(SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format_ARGB32_Premultiplied)
{
    image.fill(Qt::white);
    tempImage.fill(Qt::transparent);

    setFixedSize(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void CanvasWidget::loadImage(const QString &filename)
{
    QImage loadedImage(filename);

    if (loadedImage.isNull())
    {
        return;
    }

    loadedImage = loadedImage.scaled(SCREEN_WIDTH, SCREEN_HEIGHT,
                                     Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
                               .convertToFormat(QImage::Format_ARGB32_Premultiplied);

    // Convert to the 4 allowed gray-values
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            const int gray = qGray(loadedImage.pixel(x, y));

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

            loadedImage.setPixel(x, y, qRgb(quantizedGray, quantizedGray, quantizedGray));
        }
    }

    image = loadedImage;

    update();
}

void CanvasWidget::clear()
{
    image.fill(Qt::white);
    update();
}

void CanvasWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(rect(), image);
    painter.drawImage(rect(), tempImage);
}

void CanvasWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_drawing = true;
        firstpoint = QPoint(event->position().x() * SCREEN_WIDTH / width(),
                             event->position().y() * SCREEN_HEIGHT / height());
        lastPoint = firstpoint;
    }
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_drawing)
        return;

    const QPoint point(event->position().x() * SCREEN_WIDTH / width(),
                       event->position().y() * SCREEN_HEIGHT / height());

    QPainter painter(&image);
    QPainter tempPainter(&tempImage);
    tempPainter.setPen(QPen(currentColor, currentSize));
    painter.setPen(QPen(currentColor, currentSize));
    tempImage.fill(Qt::transparent);
    switch (::currentTool())
    {
        case Tool::Pen: // Pen
            painter.drawLine(lastPoint, point);
            break;
        case Tool::Line: // Line
            tempPainter.drawLine(firstpoint, point);
            break;
        case Tool::Rectangle: // Rectangle
            tempPainter.fillRect(QRect(firstpoint, point), currentColor);
            break;
        case Tool::Circle: // Circle
            tempPainter.drawEllipse(QRect(firstpoint, point));
            break;
        default:
            break;
    }
    lastPoint = point;
    update();
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QPainter painter(&image);
        const QPoint point(event->position().x() * SCREEN_WIDTH / width(),
                           event->position().y() * SCREEN_HEIGHT / height());

        painter.setPen(QPen(currentColor, currentSize));

        switch (::currentTool())
        {
        case Tool::Pen: // Pen
            painter.drawLine(lastPoint, point);
            break;
        case Tool::Line: // Line
            painter.drawLine(firstpoint, point);
            break;
        case Tool::Rectangle: // Rectangle
            painter.fillRect(QRect(firstpoint, point), currentColor);
            break;
        case Tool::Circle: // Circle
            painter.drawEllipse(QRect(firstpoint, point));
            break;
        default:
            break;
        }
        tempImage.fill(Qt::transparent);
        lastPoint = point;
        m_drawing = false;
        update();
    }
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
            const int gray = qGray(image.pixel(x, y));  // Convert Color to Gray (0 to 255)

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