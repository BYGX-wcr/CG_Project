#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <QGraphicsRectItem>
#include "shape.h"
#include "clippeditem.h"

class Rectangle : public Shape, public QGraphicsRectItem, public ClippedItem
{
protected:
    QPointF fixedPoint;
    QRectF originRect;
    Qt::PenStyle psFlag;

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public:
    Rectangle(Qt::PenStyle psFlag = Qt::SolidLine);

    void start(QGraphicsSceneMouseEvent *event);
    void renew(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void rotate(int angel);
    void vflip();
    void hflip();
    void scale(qreal factor);

    QList<QGraphicsItem *> clip(QRectF clipRect);
};

#endif // RECTANGLE_H
