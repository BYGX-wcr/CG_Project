#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QGraphicsScene>
#include <QWidget>
#include <QDebug>
#include "shape.h"
#include "line.h"
#include "ellipse.h"
#include "rectangle.h"
#include "polygon.h"

class PaintWidget : public QGraphicsScene
{
    Q_OBJECT

public:
    PaintWidget(QWidget *parent);

    void setCurrentShape(Shape::Type arg);
    void setCurrentPenColor(QPalette arg);
    void setCurrentBrushColor(QPalette arg);
    void setPenWidth(int arg);
    void toggleBrushState(bool checked);

    int getPenWidth();
    QColor getPenColor();
    QColor getBrushColor();
    bool getBrushStatus();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    Shape::Type currentShape;
    QColor currentPenColor;
    QColor currentBrushColor;
    int penWidth;
    Shape* currentItem;

    //Flags
    bool drawing;
    bool enBrush;
};

#endif // PAINTWIDGET_H
