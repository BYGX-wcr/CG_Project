#include "line.h"
#include "paintwidget.h"

Line::Line()
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

void Line::start(QGraphicsSceneMouseEvent *event)
{
    setLine(QLineF(mapFromScene(event->scenePos()), mapFromScene(event->scenePos())));
}

void Line::renew(QGraphicsSceneMouseEvent* event)
{
    setLine(QLineF(line().p1(), mapFromScene(event->scenePos())));
    prepareGeometryChange();
}

void Line::rotate(int angel)
{
    rotateAngel = angel;

    qreal x1 = line().x1();
    qreal y1 = line().y1();
    qreal x2 = line().x2();
    qreal y2 = line().y2();
    qreal cx = (x1 + x2) / 2;
    qreal cy = (y1 + y2) / 2;

    //setLine(cx + (x1 - cx) * cos(rotateAngel) - (y1 - cy) * sin(rotateAngel), cy + (x1 - cx) * sin(rotateAngel) + (y1 - cy) * cos(rotateAngel), cx + (x2 - cx) * cos(rotateAngel) - (y2 - cy) * sin(rotateAngel), cy + (x2 - cx) * sin(rotateAngel) + (y2 - cy) * cos(rotateAngel));
    setLine(QLineF(Shape::rotatePoint(rotateAngel, cx, cy, x1, y1), Shape::rotatePoint(rotateAngel, cx, cy, x2, y2)));

    prepareGeometryChange();
}

void Line::vflip()
{
    qreal axis;
    if (line().y1() < line().y2())
    {
        axis = line().y2();
        setLine(line().x2(), line().y2(), line().x1(), 2 * axis - line().y1());
    }
    else
    {
        axis = line().y1();
        setLine(line().x1(), line().y1(), line().x2(), 2 * axis - line().y2());
    }

    prepareGeometryChange();
}

void Line::hflip()
{
    qreal axis;
    if (line().x1() < line().x2())
    {
        axis = line().x2();
        setLine(line().x2(), line().y2(), 2 * axis - line().x1(), line().y1());
    }
    else
    {
        axis = line().x1();
        setLine(line().x1(), line().y1(), 2 * axis - line().x2(), line().y2());
    }

    prepareGeometryChange();
}

void Line::scale(qreal factor)
{
    qreal cx = line().center().x();
    qreal cy = line().center().y();
    setLine(QLineF(Shape::scalePoint(scaling, factor, cx, cy, line().p1()), Shape::scalePoint(scaling, factor, cx, cy, line().p2())));
    scaling = factor;
    prepareGeometryChange();
}

void Line::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(initPen());

    qreal x1 = mapToParent(line().p1()).x();
    qreal y1 = mapToParent(line().p1()).y();
    qreal x2 = mapToParent(line().p2()).x();
    qreal y2 = mapToParent(line().p2()).y();

    Shape::drawLine(painter, x1, y1, x2, y2);
    if (this->selected)
    {
        Shape::drawMarkPoint(painter, x1, y1);
        Shape::drawMarkPoint(painter, x2, y2);
    }
}

void Line::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene* parent = scene();
    PaintWidget* paintWidget = dynamic_cast<PaintWidget*> (parent);
    if (!selected)
    {
        if (paintWidget == NULL) return;

        if (!paintWidget->isMultiSelecting()) paintWidget->clearSelectedShapes();
        paintWidget->addSelectedShape(this);
        paintWidget->setCurrentShape(this);
        selected = true;
    }
    paintWidget->setCurrentShape(this);

    if (event->button() == Qt::LeftButton)
    {//select this item & move
        setCursor(Qt::ClosedHandCursor);
        setEditFlag(Shape::Moving);
        originLine = line();
    }
    else if (event->button() == Qt::RightButton)
    {
        if (Shape::eulicdeanDistance(mapFromScene(event->scenePos()), line().p1()) <= lineWidth)
        {
            setCursor(Qt::CrossCursor);
            setEditFlag(Shape::VertexEditing);
            fixedPoint = line().p2();
        }
        else if (Shape::eulicdeanDistance(mapFromScene(event->scenePos()), line().p2()) <= lineWidth)
        {
            setCursor(Qt::CrossCursor);
            setEditFlag(Shape::VertexEditing);
            fixedPoint = line().p1();
        }
    }
    prepareGeometryChange();
}
void Line::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (this->editFlag == Shape::Moving)
    {
        if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length() < QApplication::startDragDistance())
        {
              return;
        }

        setLine(QLineF(originLine.p1() + (event->scenePos() - event->buttonDownScenePos(Qt::LeftButton)), originLine.p2() + (event->scenePos() - event->buttonDownScenePos(Qt::LeftButton))));
        prepareGeometryChange();
    }
    else if (this->editFlag == Shape::VertexEditing)
    {
        if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::RightButton)).length() < QApplication::startDragDistance())
        {
              return;
        }

        setLine(QLineF(mapFromScene(event->scenePos()), fixedPoint));
        prepareGeometryChange();
    }
}
void Line::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::ArrowCursor);
    clearEditFlag();
    prepareGeometryChange();
}
