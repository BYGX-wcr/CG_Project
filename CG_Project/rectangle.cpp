#include "rectangle.h"
#include "paintwidget.h"

Rectangle::Rectangle()
{
}

void Rectangle::start(QGraphicsSceneMouseEvent *event)
{
    fixedPoint = mapFromScene(event->scenePos());
    setRect(mapFromScene(event->scenePos()).x(), mapFromScene(event->scenePos()).y(), 0, 0);
}

void Rectangle::renew(QGraphicsSceneMouseEvent *event)
{
    QPointF newPoint = mapFromScene(event->scenePos());
    qreal w = fabs(fixedPoint.x() - newPoint.x());
    qreal h = fabs(fixedPoint.y() - newPoint.y());

    setRect(MIN_VALUE(fixedPoint.x(), newPoint.x()), MIN_VALUE(fixedPoint.y(), newPoint.y()), w, h);
    originRect = rect();
    prepareGeometryChange();
}

void Rectangle::rotate(int angel)
{
    rotateAngel += angel;

    qreal cx = (rect().left() + rect().right()) / 2;
    qreal cy = (rect().top() + rect().bottom()) / 2;
    setTransformOriginPoint(cx, cy);
    setRotation(rotateAngel);
    prepareGeometryChange();
}

void Rectangle::vflip()
{
    qreal axis = rect().bottomRight().y();
    setRect(rect().left(), axis, rect().width(), rect().height());
    prepareGeometryChange();
}

void Rectangle::hflip()
{
    qreal axis = rect().bottomRight().x();
    setRect(axis, rect().top(), rect().width(), rect().height());
    prepareGeometryChange();
}

void Rectangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(initPen());

    //painter->rotate(-rotation());
    rotateAngel = 0;
    originRect = rect();
    qreal x1 = (originRect.topLeft()).x();
    qreal y1 = (originRect.topLeft()).y();
    qreal x2 = (originRect.bottomRight()).x() - 1;
    qreal y2 = (originRect.bottomRight()).y() - 1;
    qreal cx = (originRect.left() + originRect.right()) / 2;
    qreal cy = (originRect.top() + originRect.bottom()) / 2;

    if (!(x1 == x2 || y1 == y2))
    {
        Shape::drawLine(painter, Shape::rotatePoint(rotateAngel, cx, cy, x1, y1), Shape::rotatePoint(rotateAngel, cx, cy, x2, y1));
        Shape::drawLine(painter, Shape::rotatePoint(rotateAngel, cx, cy, x1, y1), Shape::rotatePoint(rotateAngel, cx, cy, x1, y2));
        Shape::drawLine(painter, Shape::rotatePoint(rotateAngel, cx, cy, x1, y2), Shape::rotatePoint(rotateAngel, cx, cy, x2, y2));
        Shape::drawLine(painter, Shape::rotatePoint(rotateAngel, cx, cy, x2, y1), Shape::rotatePoint(rotateAngel, cx, cy, x2, y2));

        if (enBrush)
        {//fill the rectangle
            painter->setPen(brushCol);
            //Scan filling Algorithm
            for (int i = x1 + 1; i < x2; ++i)
            {
                Shape::drawLine(painter, Shape::rotatePoint(rotateAngel, cx, cy, i, y1 + 1), Shape::rotatePoint(rotateAngel,cx, cy, i, y2 - 1));
            }
        }

        if (selected)
        {
            QPointF p1 = Shape::rotatePoint(rotateAngel, cx, cy, x1, y1);
            QPointF p2 = Shape::rotatePoint(rotateAngel, cx, cy, x1, y2);
            QPointF p3 = Shape::rotatePoint(rotateAngel, cx, cy, x2, y1);
            QPointF p4 = Shape::rotatePoint(rotateAngel, cx, cy, x2, y2);
            Shape::drawMarkPoint(painter, p1.x(), p1.y());
            Shape::drawMarkPoint(painter, p2.x(), p2.y());
            Shape::drawMarkPoint(painter, p3.x(), p3.y());
            Shape::drawMarkPoint(painter, p4.x(), p4.y());
        }
    }
    rotateAngel = rotation();
}

void Rectangle::mousePressEvent(QGraphicsSceneMouseEvent *event)
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
        originRect= rect();
    }
    else if (event->button() == Qt::RightButton)
    {
        QPointF eventPoint = mapFromScene(event->scenePos());
        if (Shape::eulicdeanDistance(eventPoint, rect().topLeft()) <= lineWidth)
        {
            setCursor(Qt::SizeFDiagCursor);
            setEditFlag(Shape::VertexEditing);
            fixedPoint = rect().bottomRight();
        }
        else if (Shape::eulicdeanDistance(eventPoint, rect().topRight()) <= lineWidth)
        {
            setCursor(Qt::SizeBDiagCursor);
            setEditFlag(Shape::VertexEditing);
            fixedPoint = rect().bottomLeft();
        }
        else if (Shape::eulicdeanDistance(eventPoint, rect().bottomRight()) <= lineWidth)
        {
            setCursor(Qt::SizeFDiagCursor);
            setEditFlag(Shape::VertexEditing);
            fixedPoint = rect().topLeft();
        }
        else if (Shape::eulicdeanDistance(eventPoint, rect().bottomLeft()) <= lineWidth)
        {
            setCursor(Qt::SizeBDiagCursor);
            setEditFlag(Shape::VertexEditing);
            fixedPoint = rect().topRight();
        }
        else if (fabs(eventPoint.x() - rect().left()) <= lineWidth)
        {
            setCursor(Qt::SizeHorCursor);
            setEditFlag(Shape::EdgeEditing);
            fixedPoint = rect().topRight();
            originRect = rect();
        }
        else if (fabs(eventPoint.x() - rect().right()) <= lineWidth)
        {
            setCursor(Qt::SizeHorCursor);
            setEditFlag(Shape::EdgeEditing);
            fixedPoint = rect().bottomLeft();
            originRect = rect();
        }
        else if (fabs(eventPoint.y() - rect().top()) <= lineWidth)
        {
            setCursor(Qt::SizeVerCursor);
            setEditFlag(Shape::EdgeEditing);
            fixedPoint = rect().bottomRight();
            originRect = rect();
        }
        else if (fabs(eventPoint.y() - rect().bottom()) <= lineWidth)
        {
            setCursor(Qt::SizeVerCursor);
            setEditFlag(Shape::EdgeEditing);
            fixedPoint = rect().topLeft();
            originRect = rect();
        }
    }
    prepareGeometryChange();
}
void Rectangle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (this->editFlag == Shape::Moving)
    {
        if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length() < QApplication::startDragDistance())
        {
              return;
        }

        QPointF p = originRect.topLeft() + (event->scenePos() - event->buttonDownScenePos(Qt::LeftButton));
        setRect(p.x(), p.y(), originRect.width(), originRect.height());
        prepareGeometryChange();
    }
    else if (this->editFlag == Shape::VertexEditing)
    {
        if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::RightButton)).length() < QApplication::startDragDistance())
        {
              return;
        }

        renew(event);
    }
    else if (this->editFlag == Shape::EdgeEditing)
    {
        if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::RightButton)).length() < QApplication::startDragDistance())
        {
              return;
        }

        QPointF eventPoint = mapFromScene(event->scenePos());
        if (fixedPoint == originRect.topRight() || fixedPoint == originRect.bottomLeft())
        {
            qreal width = fabs(eventPoint.x() - fixedPoint.x());
            setRect(MIN_VALUE(fixedPoint.x(), eventPoint.x()), originRect.top(), width, originRect.height());
        }
        else
        {
            qreal height = fabs(eventPoint.y() - fixedPoint.y());
            setRect(originRect.left(), MIN_VALUE(eventPoint.y(), fixedPoint.y()), originRect.width(), height);
        }

        prepareGeometryChange();
    }
}
void Rectangle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::ArrowCursor);
    clearEditFlag();
    prepareGeometryChange();
}
