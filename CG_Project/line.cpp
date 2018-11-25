#include "line.h"

Line::Line()
{

}

void Line::start(QGraphicsSceneMouseEvent *event)
{
    setLine(QLineF(mapFromScene(event->scenePos()), mapFromScene(event->scenePos())));
}

void Line::renew(QGraphicsSceneMouseEvent* event)
{
    setLine(QLineF(line().p1(), mapFromScene(event->scenePos())));
    prepareGeometryChange();
}

void Line::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(initPen());

    qreal x1 = mapToParent(line().p1()).x();
    qreal y1 = mapToParent(line().p1()).y();
    qreal x2 = mapToParent(line().p2()).x();
    qreal y2 = mapToParent(line().p2()).y();

    Shape::drawLine(painter, x1, y1, x2, y2);
}
