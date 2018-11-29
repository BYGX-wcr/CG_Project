#ifndef RECTANGLE_H
#define RECTANGLE_H


#include <QGraphicsRectItem>
#include "shape.h"

class Rectangle : public Shape, public QGraphicsRectItem
{
protected:
    QPointF startPoint;

public:
    Rectangle();

    void start(QGraphicsSceneMouseEvent *event);
    void renew(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void rotate(int angel);
    void vflip();
    void hflip();
};

#endif // RECTANGLE_H