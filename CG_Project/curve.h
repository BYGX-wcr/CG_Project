#ifndef CURVE_H
#define CURVE_H

#include <QGraphicsPolygonItem>
#include "shape.h"

class Curve : public Shape, public QGraphicsPolygonItem
{
protected:
    QVector<QPointF> vertexList;
    QPointF tempPoint;
    bool fixedFlag;
    int editPoint;
    QVector<QPointF> originVertex;

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public:
    Curve();

    void start(QGraphicsSceneMouseEvent* event);
    void renew(QGraphicsSceneMouseEvent * event);
    void addVertex(QGraphicsSceneMouseEvent *event);

    void rotate(int angel);
    void vflip();
    void hflip();
    void scale(qreal factor);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void drawBezierCurve(QPainter *painter, const QVector<QPointF>& nodeList);
};

#endif // CURVE_H
