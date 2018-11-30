#ifndef POLYGON_H
#define POLYGON_H

#include "shape.h"
#include <QGraphicsPolygonItem>
#include <QVector>

class Polygon : public Shape, public QGraphicsPolygonItem
{
protected:
    QVector<QPointF> vertexList;
    QPointF tempPoint;
    //QPointF* editPoint;
    //QVector<QPointF>::iterator editPoint;
    int editPoint;
    QVector<QPointF> originVertex;

    struct Edge
    {
        qreal maxY;
        qreal bottomX;
        qreal slope;

        Edge(qreal ty, qreal bx, qreal k)
        {
            maxY = ty;
            bottomX = bx;
            slope = k;
        }
        bool operator <(const Edge& right)
        {
            return bottomX < right.bottomX;
        }
    };

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public:
    Polygon();

    void start(QGraphicsSceneMouseEvent *event);
    void renew(QGraphicsSceneMouseEvent *event);
    void addVertex(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void fillPolygon(QPainter *painter);

    void rotate(int angel);
    void vflip();
    void hflip();

    bool isOcclusive();
};

#endif // POLYGON_H
