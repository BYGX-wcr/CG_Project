#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QGraphicsScene>
#include <QKeyEvent>
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

    void setCurrentTool(Shape::Type arg);
    void setCurrentPenColor(QPalette arg);
    void setCurrentBrushColor(QPalette arg);
    void setPenWidth(int arg);
    void setRotateAngel(int arg);
    void toggleBrushState(bool checked);
    void rotateShapes();
    void vflipShapes();
    void hflipShapes();

    int getPenWidth();
    QColor getPenColor();
    QColor getBrushColor();
    bool getBrushStatus();

    void clearSelectedShapes();
    void addSelectedShape(Shape* arg);
    void setCurrentShape(Shape* arg);
    bool isMultiSelecting();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    Shape::Type currentTool;
    QColor currentPenColor;
    QColor currentBrushColor;
    int penWidth;
    Shape* currentShape;
    QList<Shape*> selectedShapes;
    int rotateAngel;

    //Flags
    bool drawing;
    bool enBrush;
    bool multiSelecting;
};

#endif // PAINTWIDGET_H
