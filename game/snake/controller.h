#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "snake.h"
#include "fruit.h"
#include "bodyPart.h"

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGuiApplication>
#include <QBrush>
#include <QKeyEvent>
#include <QPointF>
#include <QString>
#include <QEvent>
#include <QApplication>
#include <QScreen>
#include <QTimer>
#include <QDBusConnection>
#include <QVector>
#include <QProcess>
#include <QtConcurrent>


QT_BEGIN_NAMESPACE
namespace Ui { class Controller; }
QT_END_NAMESPACE

class Controller : public QMainWindow
{
    Q_OBJECT

public:
    Controller(QWidget *parent = nullptr);
    ~Controller();



public slots:
    void newDirection(int direction);
    int getTargetSize();
    int getAtTarget(int position);
    int getInputsSize();
    int getAtInputs(int position);
    int getAtDistance(int in);
    double getThreshold();
    int isPredict();
    int getEpochs();
    void runGame();
    void setLabelError(double err, int epoch);

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_btnInit_clicked();
    void on_btnReset_clicked();
    void on_btnExit_clicked();
    void on_btnTrain_clicked();
    void on_btnPlayIA_clicked();
    void on_btnAuto_clicked();
    void updateSnake();

private:
    void setDirections();
    void initializeGraphics();
    void addObject(QGraphicsItem *object);
    void initializeGameObjects();

    void initializeMovement();
    void moveSnake();
    bool snakeCollapseFruit();
    bool hintWall();
    bool cannibalism();
    void setMoveDirection(bool left, bool up, bool right, bool down, const int moveDirection);
    int direction2int(QPoint);
    void debugTranslate(int);

    void gameOver();
    void resetGame();

    /* Red Neuronal */
    void addTurnTarget(int key);
    void addFrontTarget();
    void getInputs();
    QVector<int> getFruitInputs();
    void createDBus();
    void runBrain();

    /* GUI */
    void bodyToScene();
    void changeScoreValue();
    void debugPosition();
    void drawBackground();
    void debugFruit();
    void setActualDirection(int dir);

    /* Atributos */
    QTimer *timer;

    Ui::Controller *ui;
    QGraphicsScene *graphicScene;  // Maneja items 2D como un Ellipse

    /* QGraphicsItems */
    Snake *snake;
    Fruit *fruit;

    /* Flags */
    bool runningGame;
    bool autoplay;

    /* Dirección actual */
    bool leftDirection,
         rightDirection,
         upDirection,
         downDirection;

    bool keyPressed,     // Se ha pulsado A o D?
         insertedInput;  // Se ha añadido un input?
    int predict;

    QVector<BodyPart*>* snakePositions;

    /** inputs
     * En C++:
     *  [1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4,]
     *
     * En python:    QPoint prevDirection;

     *  [[1,2,3,4],[1,2,3,4],[1,2,3,4],[1,2,3,4],[1,2,3,4]]
     */
    QVector<int> *inputs;

    /** targetKey
     * En C++:
     *  [1,2,3,1,2,3,1,2,3,1,2,3,1,2,3]
     *
     * En python:
     *  [[1,2,3],[1,2,3],[1,2,3],[1,2,3],[1,2,3]]
     */
    QVector<char> *targetKey;
    QVector<int> lastDistance;
    QPoint prevDirection;
    int timerId;
    QPoint directions[TOTAL];
    QPoint direction;
    QProcess *myProcess;
};
#endif
