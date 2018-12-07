#ifndef LINE_H
#define LINE_H

#include <QGraphicsLineItem>
#include <QPainter>
#include "shape.h"

class Line : public Shape, public QGraphicsLineItem
{
protected:
    QLineF originLine;
    QPointF fixedPoint;

public:
    Line();

    void start(QGraphicsSceneMouseEvent *event);
    void renew(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void rotate(int angel);
    void vflip();
    void hflip();
    void scale(qreal factor);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // LINE_H
