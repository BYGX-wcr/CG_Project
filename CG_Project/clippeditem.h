#ifndef CLIPPEDITEM_H
#define CLIPPEDITEM_H

#include <QGraphicsItem>

class ClippedItem
{
protected:

public:
    ClippedItem();

    virtual QList<QGraphicsItem *> clip(QRectF clipRect) = 0;
};

#endif // CLIPPEDITEM_H
