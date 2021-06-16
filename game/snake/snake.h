#ifndef SNAKE_H
#define SNAKE_H

#define SNAKE_INIT_LENGHT 4

#include "common.h"
#include "bodyPart.h"

#include <QMainWindow>
#include <QVector>
#include <QPoint>
#include <QRandomGenerator>
#include <ctime>

class Snake
{

public:
    Snake();
    ~Snake();

    int getScore();
    void increaseScore();
    void resetScore();
    QVector<BodyPart*>*getBody();
    void move(QPoint direction);
    void moveAndEat();
    void eatAndMove(QPoint direction);
    QPoint getHeadPosition();
    void initFirstPositions();
    int getHead();
    int getSize();
    void initMovement(bool *upLeft, bool *upRight, bool *downLeft, bool *downRight);
    QVector<int>* getInputs();
    QVector<int> getLastDistance();
    void calcWallDistance(QPoint direction);
    void addFruitInputs(QVector<int> fruitInputs);
    QVector<int> human2snake(QVector<int> snake, QPoint direction);

private:
    void randomPosition();
    int mulOfBodySize(int num);

    int head, tail;
    QVector<BodyPart*> body;

    /** distances:
     *
     * En C++:
     *  [1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4]
     *
     * En python:
     *  [[1,2,3,4],[1,2,3,4],[1,2,3,4],[1,2,3,4],[1,2,3,4]]
     */
    QVector<int> *distances;
    int score;
};

#endif
