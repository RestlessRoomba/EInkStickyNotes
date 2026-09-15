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
    void setTool(char tool);
    void setColor(const QColor &color);
    void setToolSize(int size);

    void toBitmap(
        uint8_t screen[SCREEN_HEIGHT][BYTES_PER_ROW]
    );

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;



private:
    QImage m_image;
    bool m_drawing = false;
    QPoint m_lastPoint;
    char m_currentTool;
    QColor currentColor;
    int currentSize = 1;
};