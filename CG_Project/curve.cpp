#include "curve.h"
#include "paintwidget.h"

Curve::Curve()
{
    fixedFlag = false;
}

void Curve::start(QGraphicsSceneMouseEvent *event)
{
}

void Curve::renew(QGraphicsSceneMouseEvent *event)
{
    tempPoint = mapFromScene(event->scenePos());
    QVector<QPointF> temp = vertexList;
    temp.push_back(tempPoint);
    QPolygonF newPoly(temp);
    setPolygon(newPoly);
    fixedFlag = false;
    prepareGeometryChange();
}

void Curve::addVertex(QGraphicsSceneMouseEvent *event)
{
    QPointF curPoint = mapFromScene(event->scenePos());
    vertexList.push_back(curPoint);

    QPolygonF newPoly(vertexList);
    setPolygon(newPoly);
    fixedFlag = true;
    prepareGeometryChange();
}

void Curve::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (vertexList.size() == 0)
        return;
    painter->setPen(initPen());

    if (!fixedFlag) vertexList.push_back(tempPoint); //when drawing, add the temporary point
    //drawBezierCurve(painter, vertexList);
    for (int i = 0; i < vertexList.size();)
    {// draw the curve by sectioning method
        if (vertexList.size() - i >= 6)
        {
            QVector<QPointF> nodeList;
            if (i > 0)
            {
                nodeList.push_back(vertexList[i - 1]);
                nodeList.push_back(vertexList[i - 1] + vertexList[i - 1] - vertexList[i - 2]);
            }
            for (int j = i; j < i + 3; ++j)
            {
                nodeList.push_back(vertexList[j]);
            }
            drawBezierCurve(painter, nodeList);
            i += 3;
        }
        else
        {
            QVector<QPointF> nodeList;
            if (i > 0)
            {
                nodeList.push_back(vertexList[i - 1]);
                nodeList.push_back(vertexList[i - 1] + vertexList[i - 1] - vertexList[i - 2]);
            }
            for (int j = i; j < vertexList.size(); ++j)
            {
                nodeList.push_back(vertexList[j]);
            }
            drawBezierCurve(painter, nodeList);
            i = vertexList.size();
        }
    }
    if (!fixedFlag) vertexList.pop_back();

    if (selected)
    {
        for (int i = 0; i < vertexList.size(); ++i)
        {
            Shape::drawMarkPoint(painter, vertexList[i].x(), vertexList[i].y());
        }
    }
}

void Curve::drawBezierCurve(QPainter *painter, const QVector<QPointF> &nodeList)
{
    qreal dist = 0;
    for (int i = 0; i < nodeList.size() - 1; ++i)
    {
        dist += Shape::eulicdeanDistance(nodeList[i], nodeList[i + 1]);
    }
    qreal u = 0;
    qreal delta = 1.0 / dist;
    while (u < 1.0)
    {
        QVector<QPointF> compBuffer = nodeList;
        for (int i = nodeList.size() - 1; i >= 1; --i)
        {//compute P(u) by iteration
            for (int j = 0; j < i; ++j)
            {
                compBuffer[j] = (1 - u) * compBuffer[j] + u * compBuffer[j + 1];
            }
        }

        painter->drawPoint(compBuffer[0]);
        u += delta;
    }
}

void Curve::rotate(int angel)
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

void Curve::vflip()
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

void Curve::hflip()
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

void Curve::scale(qreal factor)
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

void Curve::mousePressEvent(QGraphicsSceneMouseEvent *event)
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

    if (fixedFlag)
    {
        if (event->button() == Qt::LeftButton)
        {//select this item & move
            setCursor(Qt::ClosedHandCursor);
            setEditFlag(Shape::Moving);
            originVertex = vertexList;
        }
        else if (event->button() == Qt::RightButton)
        {
            for (int i = 0; i < vertexList.size(); ++i)
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
void Curve::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (fixedFlag)
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
            setPolygon(QPolygonF(vertexList));
            prepareGeometryChange();
        }
    }
}
void Curve::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::ArrowCursor);
    clearEditFlag();
    prepareGeometryChange();
}
