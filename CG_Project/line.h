#ifndef LINE_H
#define LINE_H

#include <QGraphicsLineItem>
#include <QPainter>
#include "shape.h"

class Line : public Shape, public QGraphicsLineItem
{
public:
    Line();

    void start(QGraphicsSceneMouseEvent *event);
    void renew(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

#endif // LINE_H
