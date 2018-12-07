#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <QGraphicsRectItem>
#include "shape.h"

class Rectangle : public Shape, public QGraphicsRectItem
{
protected:
    QPointF fixedPoint;
    QRectF originRect;

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public:
    Rectangle();

    void start(QGraphicsSceneMouseEvent *event);
    void renew(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void rotate(int angel);
    void vflip();
    void hflip();
    void scale(qreal factor);
};

#endif // RECTANGLE_H
