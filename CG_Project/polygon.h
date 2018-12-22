#ifndef POLYGON_H
#define POLYGON_H

#include "shape.h"
#include "clippeditem.h"
#include <QGraphicsPolygonItem>
#include <QVector>

class Polygon : public Shape, public QGraphicsPolygonItem, public ClippedItem
{
protected:
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

    QVector<QPointF> vertexList;
    QPointF tempPoint;
    int editPoint;
    QVector<QPointF> originVertex;

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public:
    Polygon();
    Polygon(QVector<QPointF> arg);

    void start(QGraphicsSceneMouseEvent *event);
    void renew(QGraphicsSceneMouseEvent *event);
    void addVertex(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void fillPolygon(QPainter *painter);

    void rotate(int angel);
    void vflip();
    void hflip();
    void scale(qreal factor);

    QList<QGraphicsItem*> clip(QRectF clipRect);

    bool isOcclusive();
};

#endif // POLYGON_H
