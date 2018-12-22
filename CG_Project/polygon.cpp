#include "polygon.h"
#include "paintwidget.h"

Polygon::Polygon()
{

}
Polygon::Polygon(QVector<QPointF> arg)
{
    vertexList = arg;
}

void Polygon::start(QGraphicsSceneMouseEvent *event)
{
    vertexList.push_back(mapFromScene(event->scenePos()));
    tempPoint = vertexList.front();
    QPolygonF newPoly(vertexList);
    setPolygon(newPoly);
}

void Polygon::renew(QGraphicsSceneMouseEvent *event)
{
    tempPoint = mapFromScene(event->scenePos());
    QVector<QPointF> temp = vertexList;
    temp.push_back(tempPoint);
    QPolygonF newPoly(temp);
    setPolygon(newPoly);
    prepareGeometryChange();
}

void Polygon::addVertex(QGraphicsSceneMouseEvent *event)
{
    QPointF curPoint = mapFromScene(event->scenePos());
    tempPoint = curPoint;
    if (vertexList.size() == 1 && Shape::eulicdeanDistance(vertexList.front(), curPoint) < lineWidth) return; //prevent from touching by mistake

    if (Shape::eulicdeanDistance(curPoint, vertexList.front()) < 5 * this->lineWidth)
    {
        vertexList.push_back(vertexList.front());
    }
    else
        vertexList.push_back(curPoint);

    QPolygonF newPoly(vertexList);
    setPolygon(newPoly);
    prepareGeometryChange();
}

void Polygon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(initPen());

    if (!isOcclusive())
    {//Incomplete
        for (int i = 0; i < vertexList.size() - 1; ++i)
        {
            Shape::drawLine(painter, vertexList[i].x(), vertexList[i].y(), vertexList[i + 1].x(), vertexList[i + 1].y());
        }
        Shape::drawLine(painter, vertexList.back().x(), vertexList.back().y(), tempPoint.x(), tempPoint.y());
    }
    else
    {//Complete
        for (int i = 0; i < vertexList.size() - 1; ++i)
        {
            Shape::drawLine(painter, vertexList[i].x(), vertexList[i].y(), vertexList[i + 1].x(), vertexList[i + 1].y());
        }

        if (enBrush)
            fillPolygon(painter);
    }

    if (selected)
    {
        for (int i = 0; i < vertexList.size() - 1; ++i)
        {
            Shape::drawMarkPoint(painter, vertexList[i].x(), vertexList[i].y());
        }
    }
}

void Polygon::fillPolygon(QPainter *painter)
{
    painter->setPen(brushCol);
    QVector<QPointF> vList = vertexList;
    for (int i = 0; i < vList.size(); ++i)
    {
        vList[i].setX(vertexList[i].toPoint().x());
        vList[i].setY(vertexList[i].toPoint().y());
    }

    QVector<QPointF>::iterator it = vList.begin();
    qreal minY = vList.front().y();
    qreal maxY = minY;
    while (it != vList.end())
    {
        minY = MIN_VALUE(minY, it->y());
        maxY = MAX_VALUE(maxY, it->y());
        it++;
    }

    //Build Edge Table
    QVector<QList<Polygon::Edge>> ET(maxY - minY);
    for (int i = 0; i < vList.size() - 1; ++i)
    {
        if (vList[i].x() == vList[i + 1].x()) continue;

        qreal bx;
        qreal ty;
        qreal by;
        if (vList[i].y() > vList[i + 1].y())
        {
            bx = vList[i + 1].x();
            ty = vList[i].y();
            by = vList[i + 1].y();
        }
        else
        {
            bx = vList[i].x();
            ty = vList[i + 1].y();
            by = vList[i].y();
        }
        qreal k = (vList[i].x() - vList[i + 1].x()) / (vList[i].y() - vList[i + 1].y());
        Edge e(ty, bx, k);
        ET[ceil(by - minY)].push_back(e);
    }
    for (int i = 0; i < ET.size(); ++i)
    {//Enforce Order in every bucket
        if (!ET[i].empty())
            sort(ET[i].begin(), ET[i].end());
    }

    //Use Active Edge Table to fill
    QList<Polygon::Edge> AET;
    for (int i = 0; i < ET.size(); ++i)
    {
        if (!ET[i].empty())
        {
            AET += ET[i];
        }

        sort(AET.begin(), AET.end());

        QList<Edge>::iterator it = AET.begin();
        while (AET.end() - it >= 2)
        {
            QList<Edge>::iterator tempIt1 = it;
            QList<Edge>::iterator tempIt2 = it + 1;
            it += 2;

            qreal x1 = tempIt1->bottomX;
            qreal x2 = tempIt2->bottomX;
            Shape::drawLine(painter, x1 , i + minY, x2, i + minY);

            //update edge pair
            if (tempIt1->maxY <= (minY + i + 1))
                AET.erase(tempIt1);
            else
                tempIt1->bottomX = tempIt1->bottomX + tempIt1->slope;

            if (tempIt2->maxY == (minY + i + 1))
                AET.erase(tempIt2);
            else
                tempIt2->bottomX = tempIt2->bottomX + tempIt2->slope;
        }
    }
}

void Polygon::rotate(int angel)
{
    rotateAngel = angel;
    QVector<QPointF>::iterator it = vertexList.begin();
    qreal cx = (boundingRect().topLeft().x() + boundingRect().bottomRight().x()) / 2;
    qreal cy = (boundingRect().topLeft().y() + boundingRect().bottomRight().y()) / 2;
    while (it != vertexList.end())
    {
        QPointF p(Shape::rotatePoint(rotateAngel, cx, cy, it->x(), it->y()));
        *it = p;
        ++it;
    }

    QPolygonF newPoly(vertexList);
    setPolygon(newPoly);
    prepareGeometryChange();
}

void Polygon::vflip()
{
    qreal axis = boundingRect().bottom();
    for (int i = 0; i < vertexList.size(); ++i)
    {
        QPointF p(vertexList[i].x(), 2 * axis - vertexList[i].y());
        vertexList[i] = p;
    }

    QPolygonF newPoly(vertexList);
    setPolygon(newPoly);
    prepareGeometryChange();
}

void Polygon::hflip()
{
    qreal axis = boundingRect().right();
    for (int i = 0; i < vertexList.size(); ++i)
    {
        QPointF p(2 * axis - vertexList[i].x(), vertexList[i].y());
        vertexList[i] = p;
    }

    QPolygonF newPoly(vertexList);
    setPolygon(newPoly);
    prepareGeometryChange();
}

void Polygon::scale(qreal factor)
{
    qreal cx = (boundingRect().topLeft().x() + boundingRect().bottomRight().x()) / 2;
    qreal cy = (boundingRect().topLeft().y() + boundingRect().bottomRight().y()) / 2;
    for (int i = 0; i < vertexList.size(); ++i)
    {
        QPointF p(Shape::scalePoint(scaling, factor, cx, cy, vertexList[i]));
        vertexList[i] = p;
    }

    setPolygon(QPolygonF(vertexList));
    scaling = factor;
    prepareGeometryChange();
}

QList<QGraphicsItem*> Polygon::clip(QRectF clipRect)
{
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
    if (vertexList.size() != 5)
    {
        QVector<ClippedItem::ClipPoint>::iterator checkPtr = next + 1;
        while (!checkPtr->flag) checkPtr++;
        if (find(listB.begin(), listB.end(), *checkPtr) < find(listB.begin(), listB.end(), *next)) delta = -1;
    }

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
            if (*next == crossingPoints.back() && vertexList.size() != 5) break;
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
                if (!polygon().containsPoint(midPoint, Qt::OddEvenFill))
                {//resolve a polygon *******************************************!!!
                    Polygon* newPoly = new Polygon;
                    cache.push_back(cache.front());
                    newPoly->setPolygon(cache);
                    newPoly->vertexList = cache;
                    newPoly->setLineWidth(this->lineWidth);
                    newPoly->setPenColor(this->penCol);
                    newPoly->Shape::setBrush(this->enBrush, this->brushCol);
                    PaintWidget* paintWidget = dynamic_cast<PaintWidget*>(this->scene());
                    if (paintWidget != NULL)
                    {//make the new inner line selected
                        newPoly->Shape::setSelected(true);
                        paintWidget->addSelectedShape(newPoly);
                    }

                    res.push_back(newPoly);
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
                Polygon* newPoly = new Polygon;
                cache.push_back(cache.front());
                newPoly->setPolygon(cache);
                newPoly->vertexList = cache;
                newPoly->setLineWidth(this->lineWidth);
                newPoly->setPenColor(this->penCol);
                newPoly->Shape::setBrush(this->enBrush, this->brushCol);
                PaintWidget* paintWidget = dynamic_cast<PaintWidget*>(this->scene());
                if (paintWidget != NULL)
                {//make the new inner line selected
                    newPoly->Shape::setSelected(true);
                    paintWidget->addSelectedShape(newPoly);
                }

                res.push_back(newPoly);
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
        Polygon* newPoly = new Polygon;
        newPoly->setPolygon(cache);
        newPoly->setLineWidth(this->lineWidth);
        newPoly->setPenColor(this->penCol);
        newPoly->Shape::setBrush(this->enBrush, this->brushCol);
        newPoly->vertexList = cache;
        PaintWidget* paintWidget = dynamic_cast<PaintWidget*>(this->scene());
        if (paintWidget != NULL)
        {//make the new inner line selected
            newPoly->Shape::setSelected(true);
            paintWidget->addSelectedShape(newPoly);
        }

        res.push_back(newPoly);
        cache.clear();
    }

    return res;
}

bool Polygon::isOcclusive()
{
    if (vertexList.size() >= 3 && vertexList.front() == vertexList.back())
        return true;
    else
        return false;
}

void Polygon::mousePressEvent(QGraphicsSceneMouseEvent *event)
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

    if (isOcclusive())
    {
        if (event->button() == Qt::LeftButton)
        {//select this item & move
            setCursor(Qt::ClosedHandCursor);
            setEditFlag(Shape::Moving);
            originVertex = vertexList;
        }
        else if (event->button() == Qt::RightButton)
        {
            for (int i = 0; i < vertexList.size() - 1; ++i)
            {
                if (Shape::eulicdeanDistance(mapFromScene(event->scenePos()), vertexList[i]) <= lineWidth)
                {//edit an vertex
                    setCursor(Qt::CrossCursor);
                    setEditFlag(Shape::VertexEditing);
                    editPoint = i;
                    return;
                }
            }
        }
    }
}
void Polygon::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (isOcclusive())
    {
        if (this->editFlag == Shape::Moving)
        {
            if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length() < QApplication::startDragDistance())
            {
                  return;
            }

            QPointF dp = (event->scenePos() - event->buttonDownScenePos(Qt::LeftButton));
            for (int i = 0; i < originVertex.size(); ++i)
            {
                vertexList[i] = originVertex[i] + dp;
            }
            setPolygon(QPolygonF(vertexList));
            prepareGeometryChange();
        }
        else if (this->editFlag == Shape::VertexEditing)
        {
            if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::RightButton)).length() < QApplication::startDragDistance())
            {
                  return;
            }

            if (editPoint < vertexList.size()) vertexList[editPoint] = mapFromScene(event->scenePos());
            if (editPoint == 0) vertexList.back() = vertexList.front();
            setPolygon(QPolygonF(vertexList));
            prepareGeometryChange();
        }
    }
}
void Polygon::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::ArrowCursor);
    clearEditFlag();
    prepareGeometryChange();
}
