#ifndef CLIPPEDITEM_H
#define CLIPPEDITEM_H

#include <QGraphicsItem>

class ClippedItem
{
protected:
    struct ClipPoint : public QPointF
    {
        bool flag;
        int ptrA;
        int ptrB;

        ClipPoint(const QPointF& p = QPointF(0, 0), bool flag = false, int ptr1 = -1, int ptr2 = -1) : QPointF(p)
        {
            this->flag = flag;
            this->ptrA = ptr1;
            this->ptrB = ptr2;
        }
    };

public:
    ClippedItem();

    virtual QList<QGraphicsItem *> clip(QRectF clipRect) = 0;
};

#endif // CLIPPEDITEM_H
