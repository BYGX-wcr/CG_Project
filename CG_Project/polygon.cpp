#include "polygon.h"
#include "paintwidget.h"

Polygon::Polygon()
{

}

void Polygon::start(QGraphicsSceneMouseEvent *event)
{
    vertexList.push_back(mapFromScene(event->scenePos()));
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
    QVector<ClipPoint> nodeList; //nodes list of the polygon
    QVector<ClipPoint> winNodeList; //nodes list of the clip window

    //initialize nodeList
    for (int i = 0; i < vertexList.size() - 1; ++i)
    {
        ClipPoint p(vertexList[i]);
        nodeList.push_back(p);
    }

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

    QVector<QVector<ClipPoint>> crossingPointsA(nodeList.size()); //nodes list of crossing points at each edge of polygon
    QVector<QVector<ClipPoint>> crossingPointsB(winNodeList.size()); //nodes list of crossing points at each edge of clip window

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
                    Line temp;
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
    sort(crossingPointsB[1].begin(), crossingPointsB[1].end(), [](const QPointF& left, const QPointF& right) { return left.x() < right.x()});
    sort(crossingPointsB[2].begin(), crossingPointsB[2].end(), [](const QPointF& left, const QPointF& right) { return left.y() < right.y()});
    sort(crossingPointsB[3].begin(), crossingPointsB[3].end(), [](const QPointF& left, const QPointF& right) { return left.x() > right.x()});
    sort(crossingPointsB[4].begin(), crossingPointsB[4].end(), [](const QPointF& left, const QPointF& right) { return left.y() > right.y()});

    //clip process
    ClipPoint next = crossingPointsB[1].front();
    do
    {
        if (next.flag)
        {//a crossing point
            if (clipRect.contains(nodeList[next.ptrA]))
            {//exit point
                //
            }
            else
            {//enter point
                //
            }
        }
    } while (next != crossingPointsB[1].front());
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
