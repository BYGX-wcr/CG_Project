#include "paintwidget.h"

PaintWidget::PaintWidget(QWidget *parent) : QGraphicsScene(parent)
{
    currentTool = Shape::Line;
    currentPenColor = Qt::black;
    currentBrushColor = Qt::blue;
    currentShape = NULL;
    penWidth = 2;
    rotateAngel = 0;
    drawing = false;
    enBrush = false;
    multiSelecting = false;
}

void PaintWidget::setCurrentTool(Shape::Type arg)
{
    currentTool = arg;
    clearSelectedShapes();
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

void PaintWidget::setRotateAngel(int arg)
{
    rotateAngel = arg;
}

void PaintWidget::toggleBrushState(bool checked)
{
    enBrush = checked;
}

void PaintWidget::rotateShapes()
{
    QList<Shape*>::iterator it = selectedShapes.begin();
    while (it != selectedShapes.end())
    {
        (*it)->rotate(rotateAngel);

        ++it;
    }

    update();
}

void PaintWidget::vflipShapes()
{
    QList<Shape*>::iterator it = selectedShapes.begin();
    while (it != selectedShapes.end())
    {
        (*it)->vflip();

        ++it;
    }

    update();
}

void PaintWidget::hflipShapes()
{
    QList<Shape*>::iterator it = selectedShapes.begin();
    while (it != selectedShapes.end())
    {
        (*it)->hflip();

        ++it;
    }

    update();
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
        if (currentTool != Shape::SelectTool)
        {
            switch(currentTool)
            {
            case Shape::Line: { Line* newItem = new Line; currentShape = newItem; addItem(newItem); break; }
            case Shape::Ellipse: { Ellipse* newItem = new Ellipse; currentShape = newItem; addItem(newItem); break; }
            case Shape::Rectangle: { Rectangle* newItem = new Rectangle; currentShape = newItem; addItem(newItem); break; }
            case Shape::Polygon: { Polygon* newItem = new Polygon; currentShape = newItem; addItem(newItem); break;}
            }

            if (currentShape != NULL)
            {//start to draw a new shape
                drawing = true;
                currentShape->start(event);
                currentShape->setPenColor(currentPenColor);
                currentShape->setLineWidth(penWidth);
                if (enBrush)
                    currentShape->setBrush(currentBrushColor);
            }
            event->accept();
        }
        else
        {
            if (currentTool == Shape::SelectTool)
            {
                if (!multiSelecting)
                {
                    currentShape = NULL;
                }
                QGraphicsScene::mousePressEvent(event);
                if (!multiSelecting && currentShape == NULL)
                {
                    clearSelectedShapes();
                }
            }
        }
    }

    update();
}
void PaintWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (drawing)
    {
        currentShape->renew(event);
    }
    else if (currentTool == Shape::SelectTool)
    {
        QGraphicsScene::mouseMoveEvent(event);
    }

    update();
}
void PaintWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (currentTool == Shape::Polygon)
    {
        Polygon* newPolygon = static_cast<Polygon *>(currentShape);
        newPolygon->addVertex(event);
        if (newPolygon->isOcclusive())
        {
            drawing = false;
        }
    }
    else if (currentTool == Shape::SelectTool)
    {
        QGraphicsScene::mouseReleaseEvent(event);
    }
    else
    {
        drawing = false;
    }

    update();
}

void PaintWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Control: {
        multiSelecting = true;
        break;
    }
    case Qt::Key_Delete: {
        QList<Shape*>::iterator it = selectedShapes.begin();
        while (it != selectedShapes.end())
        {
            QGraphicsItem *item = dynamic_cast<QGraphicsItem*>(*it);
            if (item != NULL)
            {
                this->removeItem(item);
                delete item;
            }
            ++it;
        }
        selectedShapes.clear();
        update();
        break;
    }
    default:
        break;
    }
}
void PaintWidget::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Control:
        multiSelecting = false;
        break;
    default:
        break;
    }
}

void PaintWidget::clearSelectedShapes() {
    for (QList<Shape*>::iterator it = selectedShapes.begin(); it != selectedShapes.end(); ++it)
    {
        (*it)->setSelected(false);
    }
    selectedShapes.clear();
}
void PaintWidget::addSelectedShape(Shape *arg)
{
    selectedShapes.push_back(arg);
}
void PaintWidget::setCurrentShape(Shape *arg)
{
    currentShape = arg;
}
bool PaintWidget::isMultiSelecting()
{
    return multiSelecting;
}
