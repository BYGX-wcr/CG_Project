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
            Shape::drawQudraPoints(painter, cx, cy, x, y);
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
            Shape::drawQudraPoints(painter, cx, cy, x, y);
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
                Shape::drawLine(painter, rx - 1, i, lx + 1, i);
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
            Shape::drawQudraPoints(painter, cx, cy, y, x);/*KEY*/
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
            Shape::drawQudraPoints(painter, cx, cy, y, x);/*KEY*/
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
                int ly = ceil(2 * cy - ty);
                Shape::drawLine(painter, i, ly + 1, i, ty - 1);
            }
        }
    }
}
