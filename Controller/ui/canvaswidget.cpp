#include "canvaswidget.h"
#include "tool.h"
#include "communication/CommunicationManager.h"

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
            int quantizedGray = convertShade(gray);

            // Use Floyd-Steinberg algorithm to raise image quality
            int error = quantizedGray - gray;
            floydSteinberg(x, y, loadedImage, error);

            loadedImage.setPixel(x, y, qRgb(quantizedGray, quantizedGray, quantizedGray));
        }
    }

    image = loadedImage;

    update();
}

int CanvasWidget::convertShade(int gray)
{
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

    return quantizedGray;
}

void CanvasWidget::floydSteinberg(int x, int y, QImage &image, int error)
{
    auto clampGray = [](int value) {
        if (value < 0)
            return 0;
        if (value > 255)
            return 255;
        return value;
    };

    int newGray;

    if (x + 1 < image.width())
    {
        newGray = qGray(image.pixel(x + 1, y));
        newGray += (-error * 7) / 16;
        image.setPixel(x + 1, y, qRgb(clampGray(newGray), clampGray(newGray), clampGray(newGray)));
    }

    if (x + 1 < image.width() && y + 1 < image.height())
    {
        newGray = qGray(image.pixel(x + 1, y + 1));
        newGray += (-error * 1) / 16;
        image.setPixel(x + 1, y + 1, qRgb(clampGray(newGray), clampGray(newGray), clampGray(newGray)));
    }

    if (y + 1 < image.height())
    {
        newGray = qGray(image.pixel(x, y + 1));
        newGray += (-error * 5) / 16;
        image.setPixel(x, y + 1, qRgb(clampGray(newGray), clampGray(newGray), clampGray(newGray)));
    }

    if (x - 1 >= 0 && y + 1 < image.height())
    {
        newGray = qGray(image.pixel(x - 1, y + 1));
        newGray += (-error * 3) / 16;
        image.setPixel(x - 1, y + 1, qRgb(clampGray(newGray), clampGray(newGray), clampGray(newGray)));
    }
}

void CanvasWidget::loadBitmap(const std::vector<std::uint8_t>& bitmap)
{
    // Create an image and fill it by decoding 2bpp packed bitmap data
    QImage loadedImage(SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format_ARGB32_Premultiplied);
    loadedImage.fill(Qt::white);

    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        for (int x = 0; x < SCREEN_WIDTH; ++x)
        {
            const int byteIndex = y * BYTES_PER_ROW + (x / 4);
            const int shift = 6 - 2 * (x % 4);
            const uint8_t packed = bitmap[byteIndex];
            const uint8_t pixelValue = (packed >> shift) & 0x03; // 0..3

            int gray;
            switch (pixelValue)
            {
                case 0: gray = BLACK; break;
                case 1: gray = DARKGRAY; break;
                case 2: gray = LIGHTGRAY; break;
                default: gray = WHITE; break;
            }

            loadedImage.setPixel(x, y, qRgb(gray, gray, gray));
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