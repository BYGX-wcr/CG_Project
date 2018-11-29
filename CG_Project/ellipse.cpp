#include "ellipse.h"

Ellipse::Ellipse()
{

}

void Ellipse::start(QGraphicsSceneMouseEvent *event)
{
    startPoint = mapFromScene(event->scenePos());
    setRect(mapFromScene(event->scenePos()).x(), mapFromScene(event->scenePos()).y(), 0, 0);
}

void Ellipse::renew(QGraphicsSceneMouseEvent *event)
{
    QPointF newPoint = mapFromScene(event->scenePos());
    qreal w = fabs(startPoint.x() - newPoint.x());
    qreal h = fabs(startPoint.y() - newPoint.y());

    setRect(MIN_VALUE(startPoint.x(), newPoint.x()), MIN_VALUE(startPoint.y(), newPoint.y()), w, h);
    prepareGeometryChange();
}

void Ellipse::rotate(int angel)
{
    rotateAngel += (angel / 180) * PI;
    setRotation(rotateAngel);
    prepareGeometryChange();
}

void Ellipse::vflip()
{
    //
}

void Ellipse::hflip()
{
    //
}

void Ellipse::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(initPen());

    qreal x1 = mapToParent(rect().topLeft()).x();
    qreal y1 = mapToParent(rect().topLeft()).y();
    qreal x2 = mapToParent(rect().bottomRight()).x();
    qreal y2 = mapToParent(rect().bottomRight()).y();

    if (x1 == x2 || y1 == y2)
        return;

    qreal cx = (x1 + x2) / 2;
    qreal cy = (y1 + y2) / 2;
    qreal xDist = fabs(x1 - x2) / 2;
    qreal yDist = fabs(y1 - y2) / 2;
    if (xDist > yDist)
    {//a ellipse with focuses on the x axis direction
        int a = xDist;
        int b = yDist;
        int a2 = pow(a, 2);
        int b2 = pow(b, 2);

        int x = 0;
        int y = b;
        int p1 = b2 - a2 * b + a2 / 4.0;
        while (b2 * x < a2 * y)
        {//area1
            QPointF point = Shape::rotatePoint(rotateAngel, cx, cy, x, y);
            Shape::drawQudraPoints(painter, cx, cy, point.x(), point.y());
            ++x;
            if (p1 >= 0)
            {
                --y;
                p1 += - 2 * a2 * y;
            }
            p1 += 2 * b2 * x + b2;
        }

        int p2 = b * (x + 0.5) * 2 + a * (y - 1) * 2 - a * b * 2;
        while (y >= 0)
        {//area2
            QPointF point = Shape::rotatePoint(rotateAngel, cx, cy, x, y);
            Shape::drawQudraPoints(painter, cx, cy, point.x(), point.y());
            --y;
            if (p2 < 0)
            {
                ++x;
                p2 += 2 * b2 * x;
            }
            p2 += -2 * a2 * y + a2;
        }

        if (enBrush)
        {//fill the ellipse
            painter->setPen(brushCol);
            //Horizontal scan filling Algorithm
            for (int i = y1 + 1; i < y2; ++i)
            {
                int rx = floor(sqrt(pow(a, 2) - pow(a * (i - cy), 2) / pow(b, 2)) + cx);
                int lx = ceil(2 * cx - rx);
                QPointF point1 = Shape::rotatePoint(rotateAngel, cx, cy, rx - 1, i);
                QPointF point2 = Shape::rotatePoint(rotateAngel, cx, cy, lx + 1, i);
                Shape::drawLine(painter, point1, point2);
            }
        }
    }
    else
    {//a ellipse with focuses on the y axis direction
        /*use symmetry transformation to calculate*/
        int a = yDist;
        int b = xDist;
        int a2 = pow(a, 2);
        int b2 = pow(b, 2);

        int x = 0;
        int y = b;
        int p1 = b2 - a2 * b + a2 / 4.0;
        while (b2 * x < a2 * y)
        {//area1
            QPointF point = Shape::rotatePoint(rotateAngel, cx, cy, y, x);
            Shape::drawQudraPoints(painter, cx, cy, point.x(), point.y());/*KEY*/
            ++x;
            if (p1 >= 0)
            {
                --y;
                p1 += - 2 * a2 * y;
            }
            p1 += 2 * b2 * x + b2;
        }

        int p2 = b * (x + 0.5) * 2 + a * (y - 1) * 2 - a * b * 2;
        while (y >= 0)
        {//area2
            QPointF point = Shape::rotatePoint(rotateAngel, cx, cy, y, x);
            Shape::drawQudraPoints(painter, cx, cy, point.x(), point.y());/*KEY*/
            --y;
            if (p2 < 0)
            {
                ++x;
                p2 += 2 * b2 * x;
            }
            p2 += -2 * a2 * y + a2;
        }

        if (enBrush)
        {//fill the ellipse
            painter->setPen(brushCol);
            //Verteical scan filling Algorithm
            for (int i = x1 + 1; i < x2; ++i)
            {
                int ty = floor(sqrt(pow(a, 2) - pow(a * (i - cx), 2) / pow(b, 2)) + cy);
                int by = ceil(2 * cy - ty);
                QPointF point1 = Shape::rotatePoint(rotateAngel, cx, cy, i, by + 1);
                QPointF point2 = Shape::rotatePoint(rotateAngel, cx, cy, i, ty - 1);
                Shape::drawLine(painter, point1, point2);
            }
        }
    }
}
