#include "polygon.h"

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
}

void Polygon::fillPolygon(QPainter *painter)
{
    painter->setPen(brushCol);
    QVector<QPointF>::iterator it = vertexList.begin();
    qreal minY = vertexList.front().y();
    qreal maxY = minY;
    while (it != vertexList.end())
    {
        minY = MIN_VALUE(minY, it->y());
        maxY = MAX_VALUE(maxY, it->y());
        it++;
    }

    //Build Edge Table
    QVector<QList<Polygon::Edge>> ET(maxY - minY);
    for (int i = 0; i < vertexList.size() - 1; ++i)
    {
        if (vertexList[i].x() == vertexList[i + 1].x()) continue;

        qreal bx;
        qreal ty;
        qreal by;
        if (vertexList[i].y() > vertexList[i + 1].y())
        {
            bx = vertexList[i + 1].x();
            ty = vertexList[i].y();
            by = vertexList[i + 1].y();
        }
        else
        {
            bx = vertexList[i].x();
            ty = vertexList[i + 1].y();
            by = vertexList[i].y();
        }
        qreal k = (vertexList[i].x() - vertexList[i + 1].x()) / (vertexList[i].y() - vertexList[i + 1].y());
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

bool Polygon::isOcclusive()
{
    if (vertexList.size() >= 3 && vertexList.front() == vertexList.back())
        return true;
    else
        return false;
}
