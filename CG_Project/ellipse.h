#ifndef ELLIPSE_H
#define ELLIPSE_H

#include <QGraphicsEllipseItem>
#include "shape.h"

class Ellipse : public Shape, public QGraphicsEllipseItem
{
protected:
    QPointF startPoint;

public:
    Ellipse();

    void renew(QGraphicsSceneMouseEvent *event);
    void start(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void rotate(int angel);
    void vflip();
    void hflip();
};

#endif // ELLIPSE_H
