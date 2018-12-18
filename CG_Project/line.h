#ifndef LINE_H
#define LINE_H

#include <QGraphicsLineItem>
#include <QPainter>
#include "clippeditem.h"
#include "shape.h"

class Line : public Shape, public QGraphicsLineItem, public ClippedItem
{
protected:
    QLineF originLine;
    QPointF fixedPoint;

public:
    Line();

    //inherited from class<Shape>
    void start(QGraphicsSceneMouseEvent *event);
    void renew(QGraphicsSceneMouseEvent *event);
    void rotate(int angel);
    void vflip();
    void hflip();
    void scale(qreal factor);

    //inherited from class<QGraphicsItem>
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    //inherited from class<ClippedItem>
    QList<QGraphicsItem *> clip(QRectF clipRect);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // LINE_H
