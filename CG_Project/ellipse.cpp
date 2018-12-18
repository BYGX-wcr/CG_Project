#include "ellipse.h"
#include "paintwidget.h"

Ellipse::Ellipse()
{

}

void Ellipse::start(QGraphicsSceneMouseEvent *event)
{
    fixedPoint = mapFromScene(event->scenePos());
    setRect(mapFromScene(event->scenePos()).x(), mapFromScene(event->scenePos()).y(), 0, 0);
}

void Ellipse::renew(QGraphicsSceneMouseEvent *event)
{
    QPointF newPoint = mapFromScene(event->scenePos());
    qreal w = fabs(fixedPoint.x() - newPoint.x());
    qreal h = fabs(fixedPoint.y() - newPoint.y());

    setRect(MIN_VALUE(fixedPoint.x(), newPoint.x()), MIN_VALUE(fixedPoint.y(), newPoint.y()), w, h);
    prepareGeometryChange();
}

void Ellipse::rotate(int angel)
{
    rotateAngel += angel;

    qreal cx = (rect().left() + rect().right()) / 2;
    qreal cy = (rect().top() + rect().bottom()) / 2;
    setTransformOriginPoint(cx, cy);
    setRotation(rotateAngel);
    prepareGeometryChange();
}

void Ellipse::vflip()
{
    qreal axis = rect().bottomRight().y();
    setRect(rect().left(), axis, rect().width(), rect().height());
    prepareGeometryChange();
}

void Ellipse::hflip()
{
    qreal axis = rect().bottomRight().x();
    setRect(axis, rect().top(), rect().width(), rect().height());
    prepareGeometryChange();
}

void Ellipse::scale(qreal factor)
{
    qreal cx = rect().center().x();
    qreal cy = rect().center().y();
    setRect(QRectF(Shape::scalePoint(scaling, factor, cx, cy, rect().topLeft()), Shape::scalePoint(scaling, factor, cx, cy, rect().bottomRight())));
    scaling = factor;
    prepareGeometryChange();
}

void Ellipse::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(initPen());
    painter->setWorldMatrixEnabled(false); //close the auto transform

    qreal x1 = mapToParent(rect().topLeft()).x();
    qreal y1 = mapToParent(rect().topLeft()).y();
    qreal x2 = mapToParent(rect().bottomRight()).x() - 1;
    qreal y2 = mapToParent(rect().bottomRight()).y() - 1;

    if (x1 == x2 || y1 == y2)
        return;

    qreal cx = (x1 + x2) / 2;
    qreal cy = (y1 + y2) / 2;
    qreal xDist = fabs(x1 - x2) / 2;
    qreal yDist = fabs(y1 - y2) / 2;
    if (xDist > yDist)
    {//a ellipse with focuses on the x axis direction
        int a = xDist;
        int b = yDist;
        int a2 = pow(a, 2);
        int b2 = pow(b, 2);

        int x = 0;
        int y = b;
        int p1 = b2 - a2 * b + a2 / 4.0;
        while (b2 * x < a2 * y)
        {//area1
            Shape::drawQudraPoints(painter, cx, cy, x, y);
            ++x;
            if (p1 >= 0)
            {
                --y;
                p1 += - 2 * a2 * y;
            }
            p1 += 2 * b2 * x + b2;
        }

        int p2 = b * (x + 0.5) * 2 + a * (y - 1) * 2 - a * b * 2;
        while (y >= 0)
        {//area2
            Shape::drawQudraPoints(painter, cx, cy, x, y);
            --y;
            if (p2 < 0)
            {
                ++x;
                p2 += 2 * b2 * x;
            }
            p2 += -2 * a2 * y + a2;
        }

        if (enBrush)
        {//fill the ellipse
            painter->setPen(brushCol);
            //Horizontal scan filling Algorithm
            for (int i = y1 + 1; i < y2; ++i)
            {
                int rx = floor(sqrt(pow(a, 2) - pow(a * (i - cy), 2) / pow(b, 2)) + cx);
                int lx = ceil(2 * cx - rx);
                QPointF point1 = Shape::rotatePoint(rotateAngel, cx, cy, rx - 1, i);
                QPointF point2 = Shape::rotatePoint(rotateAngel, cx, cy, lx + 1, i);
                Shape::drawLine(painter, point1, point2);
            }
        }
    }
    else
    {//a ellipse with focuses on the y axis direction
        /*use symmetry transformation to calculate*/
        int a = yDist;
        int b = xDist;
        int a2 = pow(a, 2);
        int b2 = pow(b, 2);

        int x = 0;
        int y = b;
        int p1 = b2 - a2 * b + a2 / 4.0;
        while (b2 * x < a2 * y)
        {//area1
            Shape::drawQudraPoints(painter, cx, cy, y, x);/*KEY*/
            ++x;
            if (p1 >= 0)
            {
                --y;
                p1 += - 2 * a2 * y;
            }
            p1 += 2 * b2 * x + b2;
        }

        int p2 = b * (x + 0.5) * 2 + a * (y - 1) * 2 - a * b * 2;
        while (y >= 0)
        {//area2
            Shape::drawQudraPoints(painter, cx, cy, y, x);/*KEY*/
            --y;
            if (p2 < 0)
            {
                ++x;
                p2 += 2 * b2 * x;
            }
            p2 += -2 * a2 * y + a2;
        }

        if (enBrush)
        {//fill the ellipse
            painter->setPen(brushCol);
            //Verteical scan filling Algorithm
            for (int i = x1 + 1; i < x2; ++i)
            {
                int ty = floor(sqrt(pow(a, 2) - pow(a * (i - cx), 2) / pow(b, 2)) + cy);
                int by = ceil(2 * cy - ty);
                QPointF point1 = Shape::rotatePoint(rotateAngel, cx, cy, i, by + 1);
                QPointF point2 = Shape::rotatePoint(rotateAngel, cx, cy, i, ty - 1);
                Shape::drawLine(painter, point1, point2);
            }
        }
    }
    if (selected)
    {
        Shape::drawMarkPoint(painter, Shape::rotatePoint(rotateAngel, cx, cy, x1, (y1 + y2) / 2));
        Shape::drawMarkPoint(painter, Shape::rotatePoint(rotateAngel, cx, cy, x2, (y1 + y2) / 2));
        Shape::drawMarkPoint(painter, Shape::rotatePoint(rotateAngel, cx, cy, (x1 + x2) / 2, y1));
        Shape::drawMarkPoint(painter, Shape::rotatePoint(rotateAngel, cx, cy, (x1 + x2) / 2, y2));
    }

    painter->setWorldMatrixEnabled(true);
}

void Ellipse::mousePressEvent(QGraphicsSceneMouseEvent *event)
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
        if (fabs(eventPoint.x() - rect().left()) <= lineWidth)
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
void Ellipse::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
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
void Ellipse::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::ArrowCursor);
    clearEditFlag();
    prepareGeometryChange();
}
