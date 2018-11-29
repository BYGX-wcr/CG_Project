#include "rectangle.h"

Rectangle::Rectangle()
{
}

void Rectangle::start(QGraphicsSceneMouseEvent *event)
{
    startPoint = mapFromScene(event->scenePos());
    setRect(mapFromScene(event->scenePos()).x(), mapFromScene(event->scenePos()).y(), 0, 0);
}

void Rectangle::renew(QGraphicsSceneMouseEvent *event)
{
    QPointF newPoint = mapFromScene(event->scenePos());
    qreal w = fabs(startPoint.x() - newPoint.x());
    qreal h = fabs(startPoint.y() - newPoint.y());

    setRect(MIN_VALUE(startPoint.x(), newPoint.x()), MIN_VALUE(startPoint.y(), newPoint.y()), w, h);
    prepareGeometryChange();
}

void Rectangle::rotate(int angel)
{
    rotateAngel += (angel / 180) * PI;
    setRotation(rotateAngel);
    prepareGeometryChange();
}

void Rectangle::vflip()
{
    //
}

void Rectangle::hflip()
{
    //
}

void Rectangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(initPen());

    qreal x_1 = mapToParent(rect().topLeft()).x();
    qreal y_1 = mapToParent(rect().topLeft()).y();
    qreal x_2 = mapToParent(rect().bottomRight()).x();
    qreal y_2 = mapToParent(rect().bottomRight()).y();

    qreal cx = (x_1 + x_2) / 2;
    qreal cy = (y_1 + y_2) / 2;
    qreal x1 = cx + (x_1 - cx) * cos(rotateAngel) - (y_1 - cy) * sin(rotateAngel);
    qreal y1 = cy + (x_1 - cx) * sin(rotateAngel) + (y_1 - cy) * cos(rotateAngel);
    qreal x2 = cx + (x_2 - cx) * cos(rotateAngel) - (y_2 - cy) * sin(rotateAngel);
    qreal y2 = cy + (x_2 - cx) * sin(rotateAngel) + (y_2 - cy) * cos(rotateAngel);
    if (x_1 == x_2 || y_1 == y_2)
    {
        Shape::drawLine(painter, x1, y1, x2, y2);
    }
    else
    {
        Shape::drawLine(painter, x1, y1, x2, y1);
        Shape::drawLine(painter, x1, y1, x1, y2);
        Shape::drawLine(painter, x1, y2, x2, y2);
        Shape::drawLine(painter, x2, y1, x2, y2);

        if (enBrush)
        {//fill the rectangle
            painter->setPen(brushCol);
            //Scan filling Algorithm
            for (int i = x_1 + 1; i < x_2; ++i)
            {
                Shape::drawLine(painter, Shape::rotatePoint(rotateAngel, cx, cy, i, y_1 + 1), Shape::rotatePoint(rotateAngel,cx, cy, i, y_2 - 1));
            }
        }
    }
}
