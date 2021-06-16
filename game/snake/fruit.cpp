#include "fruit.h"

/**
 * @brief Fruit::Fruit constructor de la fruta
 * @param parent
 */
Fruit::Fruit(QGraphicsEllipseItem *parent) : QGraphicsEllipseItem(parent)
{
    initializeFruit();
}

Fruit::~Fruit(){}

/**
 * @brief Fruit::initializeFruit inicializa la fruta con un color de fondo
 * y un trazado en la posición 0 0.
 */
void
Fruit::initializeFruit()
{
    QPen   *pen   = new QPen(Qt::white);
    QBrush *brush = new QBrush(FRUIT_COLOR, Qt::SolidPattern);

    setBrush(*brush);
    setPen(*pen);
    setRect(0, 0, FRUIT_SIZE, FRUIT_SIZE);
#ifdef __DEBUG_FRUIT__
    fr = 0;
#endif
    delete pen;
    delete brush;

}

/**
 * @brief Fruit::moveFruit mueve la fruta a una nueva posición y comprueba si
 * es distinta a la de alguna parte del cuerpo de la serpiente, si coincide
 * se vuelve a generar.
 * @param snake
 */
void
Fruit::moveFruit(QVector<BodyPart*> *snake)
{
    QPoint *pos;
    do
    {
        pos = new QPoint();
        randomPos(pos);
        setPos(*pos);

#ifdef __DEBUG_FRUIT__
        if (fr == 5)
        {
            pos->setX(snake->at(0)->x());
            pos->setY(snake->at(0)->y());
            setPos(*pos);
            fr = 0;
        }
        fr++;
#endif
        delete pos;
    }
    while( samePosition(snake) );
}

/**
 * @brief Fruit::randomPos genera una posición aleatoria para la fruta.
 * @param position
 */
void
Fruit::randomPos(QPoint *position)
{
    timespec timeSeed;
    QRandomGenerator *rand;
    int newFruitX, newFruitY;

    /* Seed para el random (se puede usar .tv_sec o .tv_nsec), segundos o nanosegundos */
    timespec_get(&timeSeed, TIME_UTC);
    rand = new QRandomGenerator(timeSeed.tv_nsec);

    newFruitX = rand->bounded(SCENE_WIDTH  - FRUIT_SIZE);
    newFruitY = rand->bounded(SCENE_HEIGHT - FRUIT_SIZE);

    position->setX(mulOfFruitSize(newFruitX));
    position->setY(mulOfFruitSize(newFruitY));

    delete rand;
}

/**
 * @brief Fruit::samePosition verifica si la posición de la fruta coincide
 * con alguna posición del cuerpo de la serpiente, si es asi retorna true, si
 * no retorna false.
 * @param snake
 * @return
 */
bool
Fruit::samePosition(QVector<BodyPart*> *snake)
{
    int fruitX, fruitY, snakeX, snakeY;
    bool equals = false;

    fruitX = x();
    fruitY = y();

    for (int i=0; i<snake->size(); i++)
    {
        snakeX = (int) snake->at(i)->x();
        snakeY = (int) snake->at(i)->y();

        if ((snakeX == fruitX) && (snakeY == fruitY))
        {
            equals = true;

#ifdef __DEBUG_FRUIT__
            qDebug("COINCIDE\n");
            qDebug("F-X: %d\
                   \rF-Y: %d\
                   \rS-X: %d\
                   \rS-Y: %d\n",
                   fruitX, fruitY,
                   snakeX, snakeY);

            qDebug("FIN\n");
#endif

            return equals;
        }
    }
    return equals;
}

/**
 * @brief Fruit::mulOfFruitSize retorna un multiplo del tamaño de la fruta
 * para que se posicione de forma correcta en el tablero.
 * @param num
 * @return
 */
int
Fruit::mulOfFruitSize(int num)
{
    return num - (num % FRUIT_SIZE);
}

//QPoint
//Fruit::getPosition()
//{
//    QPoint position;

//    position.x() = pos.x();
//    position.y() = pos.y();

//    return position;
//}
