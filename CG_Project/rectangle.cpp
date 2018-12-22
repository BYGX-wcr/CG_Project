#include "rectangle.h"
#include "paintwidget.h"
#include <QGraphicsPolygonItem>

Rectangle::Rectangle(Qt::PenStyle psFlag)
{
    this->psFlag = psFlag;
    QPen curPen;
    curPen.setStyle(psFlag);
    this->setPen(curPen);
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

    qreal cx = rect().center().x();
    qreal cy = rect().center().y();
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

void Rectangle::scale(qreal factor)
{
    qreal cx = rect().center().x();
    qreal cy = rect().center().y();
    setRect(QRectF(Shape::scalePoint(scaling, factor, cx, cy, rect().topLeft()), Shape::scalePoint(scaling, factor, cx, cy, rect().bottomRight())));
    scaling = factor;
    prepareGeometryChange();
}

void Rectangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (psFlag != Qt::SolidLine)
    {//indicate that this is a special rect, don't use self-defined function to paint it
        QGraphicsRectItem::paint(painter, option, widget);
        return;
    }

    painter->setPen(initPen());
    painter->setWorldMatrixEnabled(false); //close the auto transform

    qreal x1 = (rect().topLeft()).x();
    qreal y1 = (rect().topLeft()).y();
    qreal x2 = (rect().bottomRight()).x() - 1;
    qreal y2 = (rect().bottomRight()).y() - 1;
    qreal cx = (x1 + x2) / 2;
    qreal cy = (y1 + y2) / 2;

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
    painter->setWorldMatrixEnabled(true);
}

QList<QGraphicsItem *> Rectangle::clip(QRectF clipRect)
{
    QVector<QPointF> vertexList;
    vertexList.push_back(this->rect().topLeft());
    vertexList.push_back(this->rect().topRight());
    vertexList.push_back(this->rect().bottomRight());
    vertexList.push_back(this->rect().bottomLeft());
    vertexList.push_back(this->rect().topLeft());
    QVector<ClippedItem::ClipPoint> nodeList; //nodes list of the polygon
    QVector<ClippedItem::ClipPoint> winNodeList; //nodes list of the clip window
    QList<QGraphicsItem*> res;
    if (vertexList.size() < 3)
        return res;

    //judge whether polygon is clockwise
    QVector<QPointF>::iterator maxIt = max_element(vertexList.begin(), vertexList.end(), [](const QPointF& left, const QPointF& right){ return left.y() > right.y(); });
    bool clockwise;
    if (maxIt == vertexList.end() || maxIt == vertexList.begin())
    {
        if (vertexList[1].x() > vertexList[vertexList.size() - 2].x())
        {
            clockwise = true;
        }
        else
        {
            clockwise = false;
        }
    }
    else
    {
        if ((maxIt + 1)->x() > (maxIt - 1)->x())
        {
            clockwise = true;
        }
        else
        {
            clockwise = false;
        }
    }

    //initialize nodeList
    for (int i = 0; i < vertexList.size() - 1; ++i)
    {
        ClipPoint p(vertexList[i]);
        nodeList.push_back(p);
    }
    if (clockwise == false)
    {//keep nodeList in clockwise
        reverse(nodeList.begin(), nodeList.end());
    }
    if (clipRect.left() < this->boundingRect().left())
    {//synchronize
        QVector<QPointF>::iterator rightIt = max_element(vertexList.begin(), vertexList.end(), [](const QPointF& left, const QPointF& right){ return left.x() < right.x(); });
        ClippedItem::ClipPoint newStart(*rightIt);
        while (nodeList[0] != newStart)
        {
            ClippedItem::ClipPoint temp = nodeList[0];
            nodeList.pop_front();
            nodeList.push_back(temp);
        }
    }
    nodeList.push_back(nodeList.front());

    //initialize winNodeList
    {
        ClipPoint p1(clipRect.topLeft());
        ClipPoint p2(clipRect.topRight());
        ClipPoint p3(clipRect.bottomRight());
        ClipPoint p4(clipRect.bottomLeft());
        winNodeList.push_back(p1);
        winNodeList.push_back(p2);
        winNodeList.push_back(p3);
        winNodeList.push_back(p4);
    }

    QVector<QVector<ClippedItem::ClipPoint>> crossingPointsA(nodeList.size() - 1); //nodes list of crossing points at each edge of polygon
    QVector<QVector<ClippedItem::ClipPoint>> crossingPointsB(winNodeList.size()); //nodes list of crossing points at each edge of clip window

    //compute crossing points
    for (int i = 0; i < winNodeList.size(); ++i)
    {
        QLineF boundary(winNodeList[i], winNodeList[(i + 1) % winNodeList.size()]);
        for (int j = 0; j < nodeList.size() - 1; ++j)
        {
            QLineF l(nodeList[j], nodeList[j + 1]);

            QPointF p;
            if (boundary.intersect(l, &p) == QLineF::BoundedIntersection)
            {//exist intersection
                bool check = true;
                if (boundary.angle() == l.angle())
                {//special check 1
                    check = false;
                }

                if (p == l.p1() || p == l.p2())
                {//special check 2
                    QGraphicsLineItem temp;
                    temp.setLine(l);
                    if (!clipRect.intersects(temp.boundingRect()))
                    {
                        check = false;
                    }
                }

                if (check)
                {
                    ClipPoint cp(p, true, j, i);
                    crossingPointsA[j].push_back(cp);
                    crossingPointsB[i].push_back(cp);
                }
            }
        }
    }

    //keep crossing points list in a clockwise order
    sort(crossingPointsB[0].begin(), crossingPointsB[0].end(), [](const ClippedItem::ClipPoint& left, const ClippedItem::ClipPoint& right) { return left.x() < right.x(); });
    sort(crossingPointsB[1].begin(), crossingPointsB[1].end(), [](const ClippedItem::ClipPoint& left, const ClippedItem::ClipPoint& right) { return left.y() < right.y(); });
    sort(crossingPointsB[2].begin(), crossingPointsB[2].end(), [](const ClippedItem::ClipPoint& left, const ClippedItem::ClipPoint& right) { return left.x() > right.x(); });
    sort(crossingPointsB[3].begin(), crossingPointsB[3].end(), [](const ClippedItem::ClipPoint& left, const ClippedItem::ClipPoint& right) { return left.y() > right.y(); });
    for (int i = 0; i < crossingPointsA.size(); ++i)
    {
        if (crossingPointsA[i].size() == 2)
        {
            qreal dx1 = nodeList[i].x() - nodeList[i + 1].x();
            qreal dy1 = nodeList[i].y() - nodeList[i + 1].y();
            qreal dx2 = crossingPointsA[i][0].x() - crossingPointsA[i][1].x();
            qreal dy2 = crossingPointsA[i][0].y() - crossingPointsA[i][1].y();
            if (dx1 * dx2 < 0)
            {
                swap(crossingPointsA[i][0], crossingPointsA[i][1]);
            }
            else if (dx1 * dx2 == 0)
            {
                if (dy1 * dy2 < 0)
                {
                    swap(crossingPointsA[i][0], crossingPointsA[i][1]);
                }
            }
        }
        else if (crossingPointsA[i].size() > 2)
        {
            qDebug() << "Unknown Error happened in Polygon::clip";
            exit(0);
        }
    }

    QVector<ClippedItem::ClipPoint> crossingPoints;
    for (int i = 0; i < crossingPointsA.size(); ++i)
    {
        for (int j = 0; j < crossingPointsA[i].size(); ++j)
        {
            crossingPoints.push_back(crossingPointsA[i][j]);
        }
    }
    if (crossingPoints.empty())
        return res;

    QVector<ClippedItem::ClipPoint> listA; //combine nodeList with crossingPointsA
    for (int i = 0; i < crossingPointsA.size(); ++i)
    {
        listA.push_back(nodeList[i]);
        for (int j = 0; j < crossingPointsA[i].size(); ++j)
        {
            listA.push_back(crossingPointsA[i][j]);
        }
    }
    QVector<ClippedItem::ClipPoint> listB; //combine winNodeList with crossingPointsB
    for (int i = 0; i < crossingPointsB.size(); ++i)
    {
        listB.push_back(winNodeList[i]);
        for (int j = 0; j < crossingPointsB[i].size(); ++j)
        {
            listB.push_back(crossingPointsB[i][j]);
        }
    }

    //clip process
    int delta = 1;
    QVector<ClippedItem::ClipPoint>::iterator next = listA.begin();
    while (!next->flag) next++;
//    QVector<ClippedItem::ClipPoint>::iterator checkPtr = next + 1;
//    while (!checkPtr->flag) checkPtr++;
//    if (find(listB.begin(), listB.end(), *checkPtr) < find(listB.begin(), listB.end(), *next)) delta = -1;

    QVector<ClippedItem::ClipPoint>::iterator last = next;
    ClippedItem::ClipPoint endPoint = *next;
    QVector<QPointF> cache;
    do
    {
        cache.push_back(*next);
        Q_ASSERT(next->flag);
        bool direction;
        const bool exitDirection = true;
        const bool enterDirection = false;
        if (clipRect.contains(nodeList[next->ptrA]))
            direction = exitDirection;
        else
        {
            int diff = crossingPointsA[next->ptrA].indexOf(*next);
            if (diff == 0)
                direction = enterDirection;
            else
                direction = exitDirection;
        }

        if (direction == exitDirection)
        {//exit point
            //trace edges of clip window update next crossing point
            //if (*next == crossingPoints.back()) break;
            next = find(listB.begin(), listB.end(), *next);
            last = next;
            next += delta;
            if (next == listB.end()) next = listB.begin();

            if (*next == endPoint)
                break;
            else if (!next->flag)
            {//deal with vertex of clipRect
                do
                {
                    cache.push_back(*next);
                    if (next == listB.begin() && delta == -1) next = listB.end();
                    next += delta;
                    if (next == listB.end()) next = listB.begin();
                } while (!next->flag);
            }
            else
            {
                QPointF midPoint((last->x() + next->x()) / 2, (last->y() + next->y()) / 2);
                if (!rect().contains(midPoint))
                {//resolve a polygon *******************************************!!!
                    cache.push_back(cache.front());
                    QGraphicsPolygonItem newPoly;
                    newPoly.setPolygon(cache);

                    Rectangle *newRect = new Rectangle;
                    newRect->setRect(newPoly.boundingRect());
                    newRect->setLineWidth(this->lineWidth);
                    newRect->setPenColor(this->penCol);
                    newRect->Shape::setBrush(this->enBrush, this->brushCol);
                    PaintWidget* paintWidget = dynamic_cast<PaintWidget*>(this->scene());
                    if (paintWidget != NULL)
                    {//make the new inner line selected
                        newRect->Shape::setSelected(true);
                        paintWidget->addSelectedShape(newRect);
                    }

                    res.push_back(newRect);
                    cache.clear();
                }
            }
        }
        else
        {//enter point
            next = find(listA.begin(), listA.end(), *next);
            last = next;
            next++;
            if (next == listA.end()) next = listA.begin();
            //trace polygon edges to update next crossing point
            while (!next->flag)
            {
                cache.push_back(*next);
                next++;
                if (next == listA.end()) next = listA.begin();
            }

            if (next < last)
            {//recover
                cache.push_back(cache.front());
                QGraphicsPolygonItem newPoly;
                newPoly.setPolygon(cache);

                Rectangle *newRect = new Rectangle;
                newRect->setRect(newPoly.boundingRect());
                newRect->setLineWidth(this->lineWidth);
                newRect->setPenColor(this->penCol);
                newRect->Shape::setBrush(this->enBrush, this->brushCol);
                PaintWidget* paintWidget = dynamic_cast<PaintWidget*>(this->scene());
                if (paintWidget != NULL)
                {//make the new inner line selected
                    newRect->Shape::setSelected(true);
                    paintWidget->addSelectedShape(newRect);
                }

                res.push_back(newRect);
                cache.clear();

                next = last;

                if (*next == crossingPoints.back()) break;
                next = find(listB.begin(), listB.end(), *next);
                last = next;
                next += delta;
                if (next == listB.end()) next = listB.begin();
            }
        }
    } while (*next != endPoint);

    if (!cache.empty())
    {//deal with the last polygon
        cache.push_back(cache.front());

        //resolve a polygon *******************************************!!!
        QGraphicsPolygonItem newPoly;
        newPoly.setPolygon(cache);

        Rectangle *newRect = new Rectangle;
        newRect->setRect(newPoly.boundingRect());
        newRect->setLineWidth(this->lineWidth);
        newRect->setPenColor(this->penCol);
        newRect->Shape::setBrush(this->enBrush, this->brushCol);
        PaintWidget* paintWidget = dynamic_cast<PaintWidget*>(this->scene());
        if (paintWidget != NULL)
        {//make the new inner line selected
            newRect->Shape::setSelected(true);
            paintWidget->addSelectedShape(newRect);
        }

        res.push_back(newRect);
        cache.clear();
    }

    return res;
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
