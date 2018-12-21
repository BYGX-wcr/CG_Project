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

QList<QGraphicsItem*> Line::clip(QRectF clipRect) //based on Liang-Barsky Algorithm
{
    QList<QGraphicsItem*> res;
    qreal dx = line().x2() - line().x1();
    qreal dy = line().y2() - line().y1();
    qreal maxY = clipRect.bottom();
    qreal minY = clipRect.top();
    qreal maxX = clipRect.right();
    qreal minX = clipRect.left();

    qreal p1 = -dx;
    qreal p2 = dx;
    qreal p3 = -dy;
    qreal p4 = dy;
    qreal q1 = line().x1() - minX;
    qreal q2 = maxX - line().x1();
    qreal q3 = line().y1() - minY;
    qreal q4 = maxY - line().y1();

    qreal u1 = 0;
    qreal u2 = 1;

    //update parameters
    if (p1 == 0 && q1 * q2 < 0)
    {//out of range
        return res;
    }
    else if (p1 < 0)
    {
        u1 = MAX_VALUE(u1, q1 / p1);
    }
    else if (p1 > 0)
    {
        u2 = MIN_VALUE(u2, q1 / p1);
    }

    if (p2 < 0)
    {
        u1 = MAX_VALUE(u1, q2 / p2);
    }
    else if (p2 > 0)
    {
        u2 = MIN_VALUE(u2, q2 / p2);
    }

    if (p3 == 0 && q3 * q4 < 0)
    {//out of range
        return res;
    }
    else if (p3 < 0)
    {
        u1 = MAX_VALUE(u1, q3 / p3);
    }
    else if (p3 > 0)
    {
        u2 = MIN_VALUE(u2, q3 / p3);
    }

    if (p4 < 0)
    {
        u1 = MAX_VALUE(u1, q4 / p4);
    }
    else if (p4 > 0)
    {
        u2 = MIN_VALUE(u2, q4 / p4);
    }

    //final computation to get the result
    if (u1 > u2)
    {//out of range
        return res;
    }
    else
    {
        QLineF l1(line().x1() + dx * u1, line().y1() + dy * u1, line().x1() + dx * u2, line().y1() + dy * u2);
        Line *innerLine = new Line;
        innerLine->setLine(l1);
        innerLine->setLineWidth(this->lineWidth);
        res.push_back(innerLine);
        PaintWidget* paintWidget = dynamic_cast<PaintWidget*>(this->scene());
        if (paintWidget != NULL)
        {//make the new inner line selected
            innerLine->Shape::setSelected(true);
            paintWidget->addSelectedShape(innerLine);
        }

//        if (u1 > 0)
//        {//the first out-of-range section
//            QLineF l2(line().x1(), line().y1(), line().x1() + dx * u1, line().y1() + dy * u1);
//            Line *outterLine = new Line;
//            outterLine->setLine(l2);
//            outterLine->setLineWidth(this->lineWidth);
//            res.push_back(outterLine);
//        }

//        if (u2 < 1)
//        {//the second out-of-range section
//            QLineF l3(line().x1() + dx * u2, line().y1() + dy * u2, line().x2(), line().y2());
//            Line *outterLine = new Line;
//            outterLine->setLine(l3);
//            outterLine->setLineWidth(this->lineWidth);
//            res.push_back(outterLine);
//        }

        return res;
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
