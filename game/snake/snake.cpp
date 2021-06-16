#include "snake.h"

/**
 * @brief Snake::Snake constructor, inicia el score a 0 y
 * las posiciones de head y tail dentro del vector del cuerpo.
 */
Snake::Snake()
{
    score = 0;
    head  = SNAKE_INIT_LENGHT - 1; // Índice del vector correspondiente a la cabeza del snake
    tail  = 0;                     // Índice del vector correspondiente a la cola del snake
    distances = new QVector<int>();
}

Snake::~Snake(){}

/**
 * @brief Snake::initFirstPositions añade tantas BodyParts como
 * el valor de la constante SNAKE_INIT_LENGHT, y ejecuta el método
 * randomPosition().
 */
void
Snake::initFirstPositions()
{
    for (int i=0; i<SNAKE_INIT_LENGHT; i++)
        body.push_back(new BodyPart(0, 0));

    randomPosition();
}

/**
 * @brief Snake::randomPosition cambia la posición de la serpiente.
 */
void
Snake::randomPosition()
{
    timespec timeSeed;
    QRandomGenerator *rand;
    int snakeX, snakeY;

    timespec_get(&timeSeed, TIME_UTC);
    rand = new QRandomGenerator(timeSeed.tv_nsec);

    snakeX = (int) rand->bounded(SCENE_WIDTH  - BODY_SIZE);
    snakeY = (int) rand->bounded(SCENE_HEIGHT - BODY_SIZE);

    for(int i=0; i<SNAKE_INIT_LENGHT; i++)
    {
        body.at(i)->setX(mulOfBodySize(snakeX)+BODY_SIZE);
        body.at(i)->setY(mulOfBodySize(snakeY));
    }
}

/**
 * @brief Snake::initMovement Inicializa el movimiento de la serpiente
 * dependiendo de la zona donde se haya generado la cabeza.
 * @param upLeft
 * @param upRight
 * @param downLeft
 * @param downRight
 */
void
Snake::initMovement(bool *upLeft,
                    bool *upRight,
                    bool *downLeft,
                    bool *downRight )
{
    int headX, headY;
    headX = body.at(head)->x();
    headY = body.at(head)->y();


    if ( headX < (SCENE_WIDTH / 2) )   // La mitad izquierda del graphicsview
    {
        if (headY <(SCENE_HEIGHT / 2)) // La mitad supierior de la izquierda
            *upLeft = true, *downRight = *upRight = *downLeft = false;
        else                           // La mitad inferior de la izquierda
            *downLeft = true, *upLeft = *upRight = *downRight = false;
    }
    else // La mitad derecha del graphicsview
    {
        if (headY <(SCENE_HEIGHT / 2)) // La mitad superior de la derecha
            *upLeft = true, *downRight = *upRight = *downLeft = false;
        else                           // La mitad inferior de la derecha
            *downRight = true, *upLeft = *upRight = *downLeft = false;
    }
}

/**
 * @brief Snake::mulOfBodySize retorna el multiplo del tamaño
 * de cada parte del cuerpo para que se pueda añadir de forma
 * correcta al QGpahicsScene
 * @param num
 * @return int
 */
int
Snake::mulOfBodySize(int num)
{
    return num - (num % BODY_SIZE);
}

/**
 * @brief Snake::getHead retorna el índice de la cabeza dentro
 * del vector de posiciones de la serpiente.
 * @return int
 */
int
Snake::getHead()
{
    return head;
}

/**
 * @brief Snake::getHeadPosition retorna la posición, x e y, de la
 * cabeza de la serpiente.
 * @return QPoint
 */
QPoint
Snake::getHeadPosition()
{ 
    return QPoint(body.at(head)->x(),
                  body.at(head)->y());
}

/**
 * @brief Snake::move actualiza el valor de la cola a la posición
 * de la nueva cabeza.
 * Incrementa la cabeza actual y dirección recibida la almacena
 * en la cola.
 * @param direction
 */
void
Snake::move(QPoint direction)
{
    QPoint headPosition = body.at(head)->getPositions();  // Comprobado

    // Cola = Posición cabeza + Dirección
    body.at(tail)->setX( headPosition.x() + direction.x()*ADVANCE );
    body.at(tail)->setY( headPosition.y() + direction.y()*ADVANCE );

    head = tail;  // Cabeza = Antigua cola
    tail++;

    if ( head == body.size() ) head = 0;
    if ( tail == body.size() ) tail = 0;

#ifdef __DEBUG_SNAKE__
    qDebug("\nHead: %i\nTail: %i", head, tail);
    for (int i=0; i<body.size(); i++)
        qDebug("snake.at(%i) = [%f, %f]", i, body.at(i)->scenePos().x(), body.at(i)->scenePos().y());
#endif
}

/**
 * @brief Snake::eatAndMove incrementa el tamaño de snake insertando una nueva
 * parte del cuerpo al vector body
 * @param direction
 */
void
Snake::eatAndMove(QPoint direction)
{
    int x = body.at(head)->x() + direction.x()*BODY_SIZE;
    int y = body.at(head)->y() + direction.y()*BODY_SIZE;


#ifdef __DEBUG_SNAKE__
    qDebug("AUX->X = %f\nAUX->Y = %f\n",aux->x(), aux->y() );
#endif

    body.insert(body.begin() + (head+1), new BodyPart(x, y));
    head++;
    tail = head+1;

    if ( head == body.size() ) head = 0;
    if ( tail == body.size() ) tail = 0;

#ifdef __DEBUG_SNAKE__
    qDebug("\nComido");
    qDebug("\nHead: %i\nTail: %i", head, tail);
    for (int i=0; i<body.size(); i++)
        qDebug("snake.at(%i) = [%f, %f]", i, body.at(i)->x(), body.at(i)->y());
#endif
}

/**
 * @brief Snake::getBody retorna todo el cuerpo de la serpiente.
 * @return QVector<BodyPart*>*
 */
QVector<BodyPart*>*
Snake::getBody()
{
    return &body;
}

/**
 * @brief Snake::getScore retorna el score actual de la serpiente.
 * @return int
 */
int
Snake::getScore()
{
    return score;
}

/**
 * @brief Snake::increaseScore incrementa el score actual en 1.
 */
void
Snake::increaseScore()
{
    score++;
}

/**
 * @brief Snake::resetScore asigna un 0 al score actual.
 */
void
Snake::resetScore()
{
    score = 0;
}

/**
 * @brief Snake::getSize retorna el tamaño del vector del cuerpo
 * de la serpiente.
 * @return int
 */
int
Snake::getSize()
{
    return body.size();
}

QVector<int>
Snake::human2snake(QVector<int> snake, QPoint direction)
{
    // Recorrer los inputs de snake
/*
 *  U D L R   Humano
 *  0 0 1 1
 *   ---> derecha
 *
 *  F B L R
 *
 *  derecha
 *  U D L R
 *  R L U D
 *
 *  izquierda ; front = snake.at(right)
 *  U D L R
 *  L R D U
 *
 *  arriba
 *  U D L R
 *  U D L R
 *
 *  abajo
 *  U D L R
 *  D U R L
 */
    const int user_up = 0, user_down = 1, user_left = 2, user_right = 3;
    int front, back, left, right;

    if (direction == QPoint(0, -1)) { // Arriba
        front = snake.at(user_up);
//        back  = snake.at(user_down);
        left  = snake.at(user_left);
        right = snake.at(user_right);
    } else if (direction == QPoint(0, 1)) {  // Abajo
        front = snake.at(user_down);
//        back  = snake.at(user_up);
        left  = snake.at(user_right);
        right = snake.at(user_left);
    } else if (direction == QPoint(-1, 0)) {  // Izquierda
        front = snake.at(user_left);
//        back  = snake.at(user_right);
        left  = snake.at(user_down);
        right = snake.at(user_up);
    } else if (direction == QPoint(1, 0)) {  // Derecha
        front = snake.at(user_right);
//        back  = snake.at(user_left);
        left  = snake.at(user_up);
        right = snake.at(user_down);
    }
    back = 1;

    QVector<int> inputsRN;
    inputsRN.append(front);
    inputsRN.append(back);
    inputsRN.append(left);
    inputsRN.append(right);

    QVector<QString> debug;
    debug.append("FRONT");
    debug.append("BACK");
    debug.append("LEFT");
    debug.append("RIGHT");
    qDebug("Vista snake:");
    for (int i=0; i<inputsRN.size(); i++) {
        qDebug(debug.at(i).toLatin1());
        qDebug("%i",   inputsRN.at(i));
    }
    //qDebug("\n");
    return inputsRN;
}

/**
 * @brief Snake::calcWallDistance calcula el tamaño de la cabeza de la serpiente
 * hacia las 4 paredes y lo añade al vector 'distances'
 */
void
Snake::calcWallDistance(QPoint direction)
{
    /*U D L R*/
    QPoint snakeHead = getHeadPosition();
    QVector<int> aux;
// ###############################################################################
    /*
     * INPUTS: 0 o 1:
     *     0 -> Puede girar
     *     1 -> No puede girar (obstáculo)
     */

    // Distancia hacia arriba
    if ( (snakeHead.y() + BODY_SIZE) / BODY_SIZE <= 1 )
        aux.append(-1);
    else
        aux.append(1);

    // Distancia hacia abajo
    if ( (SCENE_HEIGHT - snakeHead.y()) / BODY_SIZE <= 1 )
        aux.append(-1);
    else
        aux.append(1);


    // Distancia hacia la izquierda
    if ( (snakeHead.x() + BODY_SIZE) / BODY_SIZE <= 1 )
        aux.append(-1);
    else
        aux.append(1);

    // Distancia hacia la derecha
    if ( (SCENE_WIDTH - snakeHead.x()) / BODY_SIZE <= 1 )
        aux.append(-1);
    else
        aux.append(1);

    distances->operator +=(human2snake(aux, direction));



#ifdef __DEBUG_SNAKE__
    for (int i=4; i>0; i--)
        qDebug("%i ", distances->at( distances->length() - i));
    qDebug("\n");
#endif
// ###############################################################################

}

/**
 * @brief Snake::addFruitInputs añade a los datos de entrada
 * a la red neuronal si está cerca de la fruta o no.
 * @param fruitInputs
 */
void
Snake::addFruitInputs(QVector<int> fruitInputs)
{
    distances->operator +=(fruitInputs);
}

/**
 * @brief Snake::getInputs retorna un puntero al vector de distancias,
 *  que para la red neuronal son los datos de entrada.
 * @return QVector<int>*
 */
QVector<int>*
Snake::getInputs()
{
    return distances;
}

/**
 * @brief Snake::getLastDistance recorre desde el final hacia delante
 * el vector distances y retorna las 4 ultimas en un vector.
 * @return QVector<int>
 */
QVector<int>
Snake::getLastDistance()
{
    QVector<int> dist;

    for (int i=8; i>0; i--) {
        int temp = distances->at( distances->length() - i);
        dist.append(temp);
    }
    return dist;
}
