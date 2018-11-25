#include "shape.h"

Shape::Shape(const QColor& penCol, int lineWidth)
{
    this->penCol = penCol;
    this->lineWidth = lineWidth;
    this->brushCol = Qt::white;
    enBrush = false;
}

void Shape::setPenColor(QColor &arg)
{
    this->penCol = arg;
}

void Shape::setLineWidth(int &arg)
{
    this->lineWidth = arg;
}

void Shape::setBrush(QColor& arg)
{
    enBrush = true;
    brushCol = arg;
}

QPen Shape::initPen()
{
    QPen curPen(penCol);
    curPen.setWidth(lineWidth);

    return curPen;
}

void Shape::drawLine(QPainter* painter, qreal x1, qreal y1, qreal x2, qreal y2)
{
    int ix1 = x1;
    int ix2 = x2;
    int iy1 = y1;
    int iy2 = y2;
    int dx = ix2 - ix1;
    int dy = iy2 - iy1;

    if (dx == 0)
    {//handle the situation where slope equals to inf;
        int delta = dy > 0 ? 1 : -1;
        for (int y = iy1; y != iy2; y += delta)
        {
            painter->drawPoint(ix1, y);
        }
        painter->drawPoint(ix1, iy2);
    }
    else if (dy == 0)
    {
        int delta = dx > 0 ? 1 : -1;
        for (int x = ix1; x != ix2; x += delta)
        {
            painter->drawPoint(x, iy1);
        }
        painter->drawPoint(ix2, iy1);
    }
    else if (abs(dy) > abs(dx))
    {//handle the situation where |slope| > 1
        if (iy2 < iy1)
        {//keep iy2 > iy1
            Shape::switchPoint(ix1, iy1, ix2, iy2);
            dx = -dx;
            dy = -dy;
        }

        int delta = 1;
        if (dx < 0)
        {
            dx = -dx; //keep dx > 0
            delta = -1;
        }

        int p = 2 * dx - dy;
        for (int y = iy1, x = ix1; y <= iy2; ++y)
        {
            painter->drawPoint(x, y);
            if (p > 0)
            {
                x += delta;
                p += 2 * (dx - dy);
            }
            else
            {
                p += 2 * dx;
            }
        }
    }
    else
    {//the situation where |slope| < 1
        if (ix2 < ix1)
        {//keep ix2 >= ix1
            Shape::switchPoint(ix1, iy1, ix2, iy2);
            dx = -dx;
            dy = -dy;
        }

        int delta = 1;
        if (dy < 0)
        {
            dy = -dy; // keep dy > 0
            delta = -1;
        }
        int p = 2 * dy - dx;
        for (int x = ix1, y = iy1; x <= ix2; ++x)
        {
            painter->drawPoint(x, y);
            if (p > 0)
            {
                y += delta;
                p += 2 * (dy - dx);
            }
            else
            {
                p += 2 * dy;
            }
        }
    }
}

void Shape::switchPoint(int &x1, int &y1, int &x2, int &y2)
{
    int x = x1;
    x1 = x2;
    x2 = x;
    int y = y1;
    y1 = y2;
    y2 = y;
}

qreal Shape::eulicdeanDistance(QPointF p1, QPointF p2)
{
    return sqrt(pow(p1.x() - p2.x(), 2) + pow(p1.y() - p2.y(), 2));
}

void Shape::drawQudraPoints(QPainter *painter, qreal cx, qreal cy, qreal x, qreal y)
{
    painter->drawPoint(x + cx, y + cy);
    painter->drawPoint(-x + cx, y + cy);
    painter->drawPoint(x + cx, -y + cy);
    painter->drawPoint(-x + cx, -y + cy);
}
