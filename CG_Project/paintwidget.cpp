#include "paintwidget.h"

PaintWidget::PaintWidget(QWidget *parent) : QGraphicsScene(parent)
{
    currentShape = Shape::Line;
    currentPenColor = Qt::black;
    currentBrushColor = Qt::blue;
    currentItem = NULL;
    penWidth = 1;
    drawing = false;
    enBrush = false;
}

void PaintWidget::setCurrentShape(Shape::Type arg)
{
    currentShape = arg;
}

void PaintWidget::setCurrentPenColor(QPalette arg)
{
    currentPenColor = arg.color(QPalette::Background);
}

void PaintWidget::setCurrentBrushColor(QPalette arg)
{
    currentBrushColor = arg.color(QPalette::Background);
}

void PaintWidget::setPenWidth(int arg)
{
    penWidth = arg;
}

void PaintWidget::toggleBrushState(bool checked)
{
    enBrush = checked;
}

int PaintWidget::getPenWidth()
{
    return penWidth;
}

QColor PaintWidget::getPenColor()
{
    return currentPenColor;
}

QColor PaintWidget::getBrushColor()
{
    return currentBrushColor;
}

bool PaintWidget::getBrushStatus()
{
    return enBrush;
}

void PaintWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!drawing)
    {//generate a new item
        switch(currentShape)
        {
        case Shape::Line: { Line* newItem = new Line; currentItem = newItem; addItem(newItem); break; }
        case Shape::Ellipse: { Ellipse* newItem = new Ellipse; currentItem = newItem; addItem(newItem); break; }
        case Shape::Rectangle: { Rectangle* newItem = new Rectangle; currentItem = newItem; addItem(newItem); break; }
        case Shape::Polygon: { Polygon* newItem = new Polygon; currentItem = newItem; addItem(newItem); break;}
        }

        if (currentItem != NULL)
        {//start to draw a new shape
            drawing = true;
            currentItem->start(event);
            currentItem->setPenColor(currentPenColor);
            currentItem->setLineWidth(penWidth);
            if (enBrush)
                currentItem->setBrush(currentBrushColor);
        }
    }

    QGraphicsScene::mousePressEvent(event);
}
void PaintWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (drawing)
    {
        currentItem->renew(event);
    }

    QGraphicsScene::mouseMoveEvent(event);
}
void PaintWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (currentShape == Shape::Polygon)
    {
        Polygon* newPolygon = static_cast<Polygon *>(currentItem);
        newPolygon->addVertex(event);
        if (newPolygon->isOcclusive())
        {
            drawing = false;
        }
    }
    else
    {
        drawing = false;
    }

    QGraphicsScene::mouseReleaseEvent(event);
}
