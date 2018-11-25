#ifndef SHAPE_H
#define SHAPE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QDebug>
#include <cmath>
#include <vector>
using namespace std;

#define MAX_VALUE(x, y) x > y ? x : y
#define MIN_VALUE(x, y) x < y ? x : y

class Shape
{
protected:
    QColor penCol;
    QColor brushCol;
    int lineWidth;
    bool enBrush;

public:
    enum Type{ Line, Ellipse, Rectangle, Polygon };

    Shape(const QColor& col = Qt::black, int lineWidth = 1);
    void setPenColor(QColor& arg);
    void setLineWidth(int& arg);
    void setBrush(QColor& arg);
    QPen initPen();

    virtual void start(QGraphicsSceneMouseEvent* event) = 0;
    virtual void renew(QGraphicsSceneMouseEvent * event) = 0;

    static void drawLine(QPainter *painter, qreal x1, qreal y1, qreal x2, qreal y2);
    static void switchPoint(int& x1, int& y1, int& x2, int& y2);
    static qreal eulicdeanDistance(QPointF p1, QPointF p2);
    static void drawQudraPoints(QPainter *painter, qreal cx, qreal cy, qreal x, qreal y);
};

#endif // SHAPE_H
