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
void Rectangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(initPen());

    qreal x1 = mapToParent(rect().topLeft()).x();
    qreal y1 = mapToParent(rect().topLeft()).y();
    qreal x2 = mapToParent(rect().bottomRight()).x();
    qreal y2 = mapToParent(rect().bottomRight()).y();
    if (x1 == x2 || y1 == y2)
    {
        Shape::drawLine(painter, x1, y1, x2, y2);
    }
    else
    {
        Shape::drawLine(painter, x1, y1, x2, y1);
        Shape::drawLine(painter, x1, y1, x1, y2);
        Shape::drawLine(painter, x1, y2, x2, y2);
        Shape::drawLine(painter, x2, y1, x2, y2);
    }

    if (enBrush)
    {//fill the rectangle
        painter->setPen(brushCol);
        //Verteical scan filling Algorithm
        for (int i = x1 + 1; i < x2; ++i)
        {
            Shape::drawLine(painter, i, y1 + 1, i, y2 - 1);
        }
    }
}
