#pragma once

#include "main.h"

#include <QWidget>
#include <QImage>
#include <QPoint>
#include <cstdint>

class CanvasWidget : public QWidget
{
public:
    explicit CanvasWidget(QWidget *parent = nullptr);

    void loadImage(const QString &filename);
    void clear();
    void setColor(const QColor &color);
    void setToolSize(int size);

    void toBitmap(std::vector<std::uint8_t>& bitmap);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;



private:
    QImage image;
    QImage tempImage;

    bool m_drawing = false;
    QPoint lastPoint;
    QPoint point;
    QPoint firstpoint;
    QColor currentColor;
    int currentSize = 1;
};