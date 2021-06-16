#ifndef FRUIT_H
#define FRUIT_H

#include "common.h"
#include "bodyPart.h"

#include <QGraphicsEllipseItem>
#include <QBrush>
#include <QPen>
#include <QPoint>
#include <QRandomGenerator>
#include <stdlib.h>
#include <time.h>

class Fruit : public QGraphicsEllipseItem
{

public:
    Fruit(QGraphicsEllipseItem *parent = nullptr);
    void moveFruit(QVector<BodyPart *> *);
//    QPoint getPosition();
    ~Fruit();    

private:
    void randomPos(QPoint *position);
    void initializeFruit();
    int mulOfFruitSize(int num);
    void setPositionFruit(QVector<BodyPart *> *snake);
    bool samePosition(QVector<BodyPart*> *snake);

#ifdef __DEBUG_FRUIT__
    int fr;
#endif

};

#endif
