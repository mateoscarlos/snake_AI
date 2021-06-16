#ifndef BODY_H
#define BODY_H

#include "common.h"

#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>
#include <QVector>


class BodyPart : public QGraphicsRectItem
{
public:
    BodyPart(int x, int y, QGraphicsRectItem *parent = nullptr);
    QPoint getPositions();
    ~BodyPart();

private:
    void formatBody();
};

#endif // BODY_H
