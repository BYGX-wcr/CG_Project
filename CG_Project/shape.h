#ifndef SHAPE_H
#define SHAPE_H

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QCursor>
#include <cmath>
#include <vector>
//#include "paintwidget.h"
using namespace std;

#define MAX_VALUE(x, y) x > y ? x : y
#define MIN_VALUE(x, y) x < y ? x : y

const qreal PI = 3.1415926;

class Shape
{
public:
    enum Type { SelectTool, LineTool, EllipseTool, RectangleTool, PolygonTool, CurveTool, ClipTool };
    enum EditFlag { Unedited, Moving, EdgeEditing, VertexEditing };

    Shape(const QColor& col = Qt::black, int lineWidth = 1);
    void setPenColor(QColor& arg);
    void setLineWidth(const int& arg);
    void setBrush(bool en, QColor& arg);
    void setSelected(bool arg);
    bool isSelected();
    void setEditFlag(const Shape::EditFlag& arg);
    void clearEditFlag();
    QPen initPen();

    virtual void start(QGraphicsSceneMouseEvent* event) = 0;
    virtual void renew(QGraphicsSceneMouseEvent * event) = 0;

    virtual void rotate(int angel) = 0;
    virtual void vflip() = 0;
    virtual void hflip() = 0;
    virtual void scale(qreal factor) = 0;

    void drawLine(QPainter *painter, qreal x1, qreal y1, qreal x2, qreal y2);
    void drawLine(QPainter *painter, QPointF p1, QPointF p2);
    void drawQudraPoints(QPainter *painter, qreal cx, qreal cy, qreal x, qreal y);
    void drawMarkPoint(QPainter *painter, QPointF p);
    void drawMarkPoint(QPainter *painter, qreal x, qreal y);
    static void switchPoint(int& x1, int& y1, int& x2, int& y2);
    static QPointF rotatePoint(qreal angel, qreal cx, qreal cy, QPointF p);
    static QPointF rotatePoint(qreal angel, qreal cx, qreal cy, qreal x, qreal y);
    static QPointF scalePoint(qreal prev, qreal cur, qreal cx, qreal cy, QPointF p);
    static QPointF scalePoint(qreal prev, qreal cur, qreal cx, qreal cy, qreal x, qreal y);
    static qreal eulicdeanDistance(QPointF p1, QPointF p2);

protected:
    QColor penCol;
    QColor brushCol;
    uint32_t lineWidth;
    qreal rotateAngel; //radient measure
    qreal scaling;
    bool enBrush;
    bool selected;
    Shape::EditFlag editFlag;
};



#endif // SHAPE_H
