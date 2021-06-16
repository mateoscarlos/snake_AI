#include "controller.h"
#include "ui_controller.h"

/**
 * @brief Controller::Controller constructor del controlador,
 * inicializa los gráficos, las direcciones, los vectores de
 * inputs y target para la red neuronal y crea el DBus.
 * @param parent
 */
Controller::Controller(QWidget *parent) : QMainWindow(parent) , ui(new Ui::Controller)
{
    ui->setupUi(this);  // Configura la interfaz para el widget indicado

    /* Centrar Ventana */
    move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center());

    initializeGraphics();
    setDirections();

    /* Vectores para entrenar la red */
    inputs = new QVector<int>();
    targetKey = new QVector<char>();
    autoplay = false;


    myProcess = new QProcess(this);
    createDBus();

    ui->btnTrain->setEnabled(false);
    ui->btnReset->setEnabled(false);
}

/**
 * @brief Controller::~Controller destructor.
 */
Controller::~Controller()
{
    delete ui;
}

/**
 * @brief Controller::drawBackground, dibuja las lineas
 * verticales y horizontales en el QGraphicsScene.
 */
void
Controller::drawBackground()
{
    /* Lineas Horizontales */
    for(int j=0; j<SCENE_HEIGHT; j+=BODY_SIZE)
        graphicScene->addLine(0, j, SCENE_WIDTH, j, QPen(BOARD_LINES_COLOR));

    /* Lineas Verticales */
    for(int j=0; j<SCENE_WIDTH; j+=BODY_SIZE)
        graphicScene->addLine(j, 0, j, SCENE_HEIGHT, QPen(BOARD_LINES_COLOR));
}

/**
 * @brief Controller::initializeMovement en función de donde se genera
 * la serpiente se configura una dirección u otra.
 */
void
Controller::initializeMovement()
{
    /* Posiciones dentro del tablero */
    bool upLeft, upRight, downLeft, downRight;
    /*
      +----------+----------+
      |          |          |   El tablero del juego.
      |  upLeft  |  upRight |
      |          |          |
      +----------+----------+
      |          |          |
      | downLeft | downRight|
      |          |          |
      +----------+----------+
     */
    snake->initMovement(&upLeft, &upRight, &downLeft, &downRight);

    if (upLeft)
        setMoveDirection(false, false, false, true, DOWN);

    else if (upRight)
        setMoveDirection(true, false, false, false, LEFT);


    else if (downLeft)
        setMoveDirection(false, true, false, false, UP);

    else if (downRight)
        setMoveDirection(true, false, false, false, LEFT);

    prevDirection = direction;
}

/**
 * @brief Controller::initializeGraphics, inicializa los elementos
 * gráficos del QGrpahicsScene y de QGraphicsView.
 */
void
Controller::initializeGraphics()
{
    ui->graphicsView->setMaximumSize(SCENE_WIDTH, SCENE_HEIGHT);
    ui->graphicsView->setMinimumSize(SCENE_WIDTH, SCENE_HEIGHT);
    ui->graphicsView->setBackgroundBrush(QBrush(BOARD_COLOR));
    ui->graphicsView->setCursor(Qt::BlankCursor);

    graphicScene = new QGraphicsScene(this);  // Manejador de items 2D.
    graphicScene->setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);

    ui->graphicsView->setScene(graphicScene); // Añadimos el Scene al Widget de Gráficos del UI.
    ui->graphicsView->setDragMode(QGraphicsView::NoDrag);  // Deshabilita el arrastre con el ratón
    drawBackground();
}

/**
 * @brief Controller::addObject, añade cualquier
 * QGraphicsItem al QGraphicsScene.
 * @param el QGraphicsItem para añadir a la
 * escena gráfica.
 */
void
Controller::addObject(QGraphicsItem *object)
{
    graphicScene->addItem(object);
}

/**
 * @brief Controller::setDirections Configura las
 * direcciones.
 */
void
Controller::setDirections()
{
    directions[0].setX(1),  directions[0].setY(0);  // Derecha
    directions[1].setX(-1), directions[1].setY(0);  // Izquierda
    directions[2].setX(0),  directions[2].setY(-1); // Arriba
    directions[3].setX(0),  directions[3].setY(1);  // Abajo
}

/**
 * @brief Controller::updateSnake Muestra en la interfaz
 * gráfica la posición de la serpiente, de la fruta y
 * comprueba se se ha chocado con la pared/fruta y mueve
 * la serpiente.
 */
void
Controller::updateSnake()
{
    debugPosition();
    debugFruit();
    snake->calcWallDistance(prevDirection);
    snake->addFruitInputs(getFruitInputs());

    insertedInput = true;

    prevDirection = direction; // Cambia la dirección anterior a Front

    /* Comparaciones para la creación del target */
    if (!keyPressed) {
        addFrontTarget();
    }

    if (keyPressed)
        keyPressed = false;

    /* Si choca con la fruta */
    if ( snakeCollapseFruit() ) {
        snake->increaseScore();
//        snake->eatAndMove(direction);
        snake->move(direction);
        addObject(snake->getBody()->at(snake->getHead()));
        changeScoreValue();
        fruit->moveFruit(snake->getBody());

    } else if (!autoplay){
        snake->move(direction);
    } else {  // Juega automáticamente:
        int fruit_x = fruit->pos().x();
        int fruit_y = fruit->pos().y();
        int snake_x = snake->getHeadPosition().x();
        int snake_y = snake->getHeadPosition().y();

#ifdef __DEBUG__
        qDebug("Fruta: [%i,%i]", fruit_x, fruit_y);
        qDebug("Snake: [%i,%i]\n", snake_x, snake_y);
#endif

        if (snake_x > fruit_x) {
            snake->move(directions[LEFT]);
        }else if (snake_x < fruit_x) {
            snake->move(directions[RIGHT]);
        } else if (snake_y < fruit_y) {
            snake->move(directions[DOWN]);
        } else if (snake_y > fruit_y) {
            snake->move(directions[UP]);
        }
    }

    lastDistance = snake->getLastDistance();

    /* Si se choca con ella misma o la pared */
    if ( cannibalism() || hintWall() ) {
        if(!autoplay)
            resetGame();
    }
}

/**
 * @brief Controller::keyPressEvent se ejecuta cuando se lanza
 * un evento de QKeyEvent y captura la tecla pulsada.
 * Si la tecla pulsada es A o D cambia la dirección de la serpiente
 * basandose en la dirección actual de la misma.
 * @param QKeyEvent event
 */
void
Controller::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();

    prevDirection = direction; // Cambia la dirección anterior a donde ha girado

    if (!event->isAutoRepeat()) {
        /* Movimientos actuales */
        if (leftDirection) {     // Se mueve hacia la Izquierda.
            if  (key == Qt::Key_A)
                setMoveDirection(false, false, false, true, DOWN);

            else if (key == Qt::Key_D)
                setMoveDirection(false, true, false, false, UP);
        }

        else if (upDirection) {  // Se mueve hacia Arriba.
            if  (key == Qt::Key_A)
                setMoveDirection(true, false, false, false, LEFT);

            else if (key == Qt::Key_D)
                setMoveDirection(false, false, true, false, RIGHT);
        }

        else if (rightDirection) { // Se mueve hacia la Derecha.
            if  (key == Qt::Key_A)
                setMoveDirection(false, true, false, false, UP);

            else if (key == Qt::Key_D)
                setMoveDirection(false, false, false, true, DOWN);
        }

        else if (downDirection) { // Se mueve hacia Abajo.
            if  (key== Qt::Key_A)
                setMoveDirection(false, false, true, false, RIGHT);

            else if (key == Qt::Key_D)
                setMoveDirection(true, false, false, false, LEFT);
        }
        if (insertedInput) {
            addTurnTarget(key);
            insertedInput = false;
        }

    }
}

/**
 * @brief Controller::addTurnTarget Añade un nuevo dato
 * al target si se ha pulsado una de las teclas 'A' o 'D'.
 * @param key: La tecla pulsada.
 */
void
Controller::addTurnTarget(int key)
{
    if ( runningGame && (key == Qt::Key_A || key == Qt::Key_D) ) {
        keyPressed = true;

        if (key == Qt::Key_A) {
            targetKey->append(1);
            targetKey->append(0);
            targetKey->append(0);
        }

        else {
            targetKey->append(0);
            targetKey->append(1);
            targetKey->append(0);
        }
    }
}

/**
 * @brief Controller::addFrontTarget añade un nuevo dato al
 * vector 'target' si no se ha pulsado la 'A' o 'D'.
 */
void
Controller::addFrontTarget()
{
    if (runningGame) {
        targetKey->append(0);
        targetKey->append(0);
        targetKey->append(1);
    }
}

/**
 * @brief Controller::setMoveDirection cambia la dirección
 * actual de la serpiente
 * @param left
 * @param up
 * @param right
 * @param down
 * @param moveDirection
 */
void
Controller::setMoveDirection(bool left,
                             bool up,
                             bool right,
                             bool down,
                             const int moveDirection)
{
    leftDirection  = left;
    upDirection    = up;
    rightDirection = right;
    downDirection  = down;
    direction = directions[moveDirection];
}

/**
 * @brief Controller::snakeCollapseFruit verifica si la serpiente
 * se ha chocado con la fruta.
 * Si se ha chocado incrementa el atributo 'score' y retorna true,
 * si no retorna false
 * @return bool
 */
bool
Controller::snakeCollapseFruit()
{
    QPoint fruitPos  = fruit->pos().toPoint();
    QPoint snakeHead = snake->getHeadPosition();

    if (snakeHead == fruitPos)
        return true;

    else return false;
}

/**
 * @brief Controller::bodyToScene añade al QGraphicsScene cada
 * elemento del cuerpo.
 */
void
Controller::bodyToScene()
{
    for(int i=0; i<snakePositions->size(); i++)
        graphicScene->addItem(snakePositions->at(i));
}

/**
 * @brief Controller::hintWall comprueba si la cabeza de la
 * serpiente se ha chocado contra la pared y devuelve true si
 * se ha chocado, si no, false.
 * @return bool
 */
bool
Controller::hintWall()
{
    int headX = snake->getHeadPosition().x();
    int headY = snake->getHeadPosition().y();

    if( (headX >= SCENE_WIDTH) || (headX < 0) )
        return true;

    else if ((headY >= SCENE_HEIGHT) || (headY < 0 ))
        return true;

    else return false;

}

/**
 * @brief Controller::cannibalism verifica si la cabeza se ha chocado
 * con alguna parte del cuerpo, si es así retorna true, si no retorna
 * false.
 * @return bool
 */
bool
Controller::cannibalism()
{
    QPoint headPosition = snake->getBody()->at(snake->getHead())->getPositions();

    for (int i=0; i<snake->getSize(); i++)
        if (i != snake->getHead())
            if (headPosition.x() == snake->getBody()->at(i)->x() &&
                    headPosition.y() == snake->getBody()->at(i)->y() )
                return true;
    return false;
}

/**
 * @brief Controller::gameOver llama al método resetGame()
 */
void
Controller::gameOver()
{
    resetGame();
}

/**
 * @brief Controller::runGame comprueba si el juego está parado
 * y si es asi lo inicia, iniciando el timer.
 */
void
Controller::runGame() {
    if (!runningGame){
        initializeGameObjects();
        timer->start(TIMER_SPEED);
        runningGame = true;
        predict = 0;
        ui->scoreLabelNum->setText("0");
    }
}

/**
 * @brief Controller::initializeGameObjects inicializa los objetos
 * snake, fruit y timer.
 * Configura el método que ejecutará el timer.
 * Añade la fruta y la serpiente al QGraphicsScene
 */
void
Controller::initializeGameObjects()
{
    snake     = new Snake();
    fruit     = new Fruit();
    timer     = new QTimer(this);

    runningGame = false;
    keyPressed  = false;

    /* Cuando el timer mande la señal timeout, lo conecto con el metodo
    *  de esta misma clase, llamado updateSnake()
    */
    connect(timer, SIGNAL(timeout()), this, SLOT(updateSnake()));

    snake->initFirstPositions();

    /* Guardamos el cuerpo de la serpiente para poder modificar sus datos en controller.cpp */
    snakePositions = snake->getBody();
    fruit->moveFruit(snakePositions);

    addObject(fruit);
    bodyToScene();

    /* Si no lanzamos el evento, la serpiente muere nada más empezar
     * a no ser que nosotros pulsemos una tecla
     */
    initializeMovement();
}


/*****************************************/
/*                                       */
/*         Slots de los botones          */
/*                                       */
/*****************************************/

/**
 * @brief Controller::on_btnInit_clicked botón de inicio
 * que llama al método runGame().
 */
void
Controller::on_btnInit_clicked()
{
    ui->btnReset->setEnabled(true);
    if (predict)
        ui->btnTrain->setEnabled(false);
    else
        ui->btnTrain->setEnabled(true);

    runGame();
}

/**
 * @brief Controller::on_btnReset_clicked botón de reinicio
 * del juego, llama al método resetGame().
 */
void
Controller::on_btnReset_clicked()
{

    resetGame();
    if( myProcess->isOpen())
        myProcess->kill();
    predict = 0;

    inputs->clear();
    targetKey->clear();

    ui->btnTrain->setEnabled(true);
    ui->btnPlayIA->setEnabled(true);
    ui->btnAuto->setEnabled(true);
    ui->btnInit->setEnabled(true);

    ui->btnTrain->setEnabled(false);
    ui->btnReset->setEnabled(false);

    ui->scoreLabelNum->setText("0");
    ui->lblInputsSize->setText("0");
    ui->lblMeanError->setText("0");
    ui->lblActEpoch->setText("0");
    ui->lblActDirection->setText("-");
    ui->inputEpoch->clear();
}

/**
 * @brief Controller::on_btnTrain_clicked botón de entrenamiento
 * que resetea el juego y llama al método runBrain().
 */
void
Controller::on_btnTrain_clicked()
{
    ui->btnTrain->setEnabled(false);
    ui->btnPlayIA->setEnabled(false);
    ui->btnInit->setEnabled(true);
    ui->btnAuto->setEnabled(false);
    resetGame();
    runBrain();
}

/**
 * @brief Controller::on_btnPlayIA_clicked botón de Play IA que configura
 * el flag de predicciones y ejecuta el método runBrain().
 */
void
Controller::on_btnPlayIA_clicked()
{
    ui->btnTrain->setEnabled(false);
    ui->btnPlayIA->setEnabled(false);
    ui->btnAuto->setEnabled(false);
    //ui->btnInit->setEnabled(false);
    ui->btnReset->setEnabled(true);
    runGame();
    predict = 1;
    runBrain();
}

/**
 * @brief Controller::on_btnExit_clicked botón salir, finaliza
 * el programa.
 */
void
Controller::on_btnExit_clicked()
{
    delete inputs;
    delete targetKey;
    exit(0);
}

/**
 * @brief Controller::on_btnAuto_clicked botón para entrenamiento automático
 */
void
Controller::on_btnAuto_clicked()
{
    runGame();
    autoplay = !autoplay;
    timer->start(TIMER_SPEED_AUTO);


    if (autoplay) {
        ui->btnInit->setEnabled(false);
        ui->btnTrain->setEnabled(true);
    }

    else
        ui->btnInit->setEnabled(true);

    ui->btnReset->setEnabled(true);
}


/**
 * @brief Controller::runBrain ejecuta la red neuronal en
 * un proceso distinto al actual.
 */
void
Controller::runBrain()
{
    QString program = "python3";
    QStringList arguments;
    arguments << "../snake_brain/main.py";

    myProcess->start(program, arguments);

    if (myProcess->waitForStarted())
        qDebug("Neural network started.");
}

/**
 * @brief Controller::resetGame si el juego está iniciado para el timer,
 * elimina los objetos snake, fruit y timer, limpia el tablero, dibuja el
 * tablero y cambia labels del ui gráfico.
 */
void
Controller::resetGame()
{
    if (runningGame){
        getInputs();
        timer->stop();

        delete snake;
        delete fruit;
        delete timer;

        graphicScene->clear();
        drawBackground();

        ui->lblSnakeX->setText("SNAKE X: 0");
        ui->lblSnakeY->setText("SNAKE Y: 0");
        ui->lblFruitX->setText("FRUIT X: 0");
        ui->lblFruitY->setText("FRUIT Y: 0");

        runningGame = false;


    }
#ifdef __DEBUG__
    qDebug("AUX->X = %f\nAUX->Y = %f\n",aux->x(), aux->y() );

    int inpt = inputs->length();
    int tar  = targetKey->length();

    qDebug("Target: %i - Regla: %i ",tar, (3 * inpt) / 4);

    for (int i=0; i<inputs->length(); i++)
        if(!inputs->at(i))
            qDebug("Hay un cero");

    qDebug("Position 0: %i", inputs->at(0));
#endif

    autoplay = false;
}

/**
 * @brief Controller::createDBus crea un bus en D-Bus con el
 * nombre de la constante DBUS_INPUTS y registra este objeto (Controller)
 * con los métodos declarados como public slots dentro de la ruta DBUS_PATH.
 */
void
Controller::createDBus()
{
    QDBusConnection::sessionBus().registerService(DBUS_INPUTS);
    QDBusConnection::sessionBus().registerObject(DBUS_PATH, this, QDBusConnection::ExportAllSlots);
}

/**
 * @brief Controller::getInputs añade al vector 'inputs' los
 * datos de entrada para la red neuronal que ha generado la clase
 * Snake.
 */
void
Controller::getInputs()
{
    inputs->operator +=(*snake->getInputs());
}

QVector<int>
Controller::getFruitInputs()
{
    QVector<int> fruitInputs;


    if (snake->getHeadPosition().y() < fruit->y()) {
        fruitInputs.append(-1); // Arriba
        fruitInputs.append(1); // Abajo
    } else if (snake->getHeadPosition().y() > fruit->y()){
        fruitInputs.append(1); // Arriba
        fruitInputs.append(-1); // Abajo
    } else {
        fruitInputs.append(-1); // Arriba
        fruitInputs.append(-1); // Abajo
    }

    if (snake->getHeadPosition().x() < fruit->x()) {
        fruitInputs.append(-1); // Izquierda
        fruitInputs.append(1); // Derecha
    } else if (snake->getHeadPosition().x() > fruit->x()){
        fruitInputs.append(1); // Izquierda
        fruitInputs.append(-1); // Derecha
    } else {
        fruitInputs.append(-1); // Izquierda
        fruitInputs.append(-1); // Derecha
    }

    return snake->human2snake(fruitInputs, prevDirection);
}

/**********************************************/
/*                                            */
/* Slots publicos que se exportan en el D-Bus */
/*                                            */
/**********************************************/

/**
 * @brief Controller::newDirection lanza un evento de tecla pulsada para
 * cambiar la dirección actual de la serpiente según la predicción de la
 * red neuronal.
 * @param direction
 */
void
Controller::newDirection(int direction)
{
    /* 1 - Izquierda
     * 2 - Derecha
     * 3 - Frente;    No se comprueba, siempre va adelante.
     */
    QKeyEvent *e;
    setActualDirection(direction);

    if (direction == 1)
        e = new QKeyEvent(QEvent::KeyPress,Qt::Key_A,Qt::NoModifier,"a");

    else if (direction == 2)
        e = new QKeyEvent(QEvent::KeyPress,Qt::Key_D,Qt::NoModifier,"d");

    if (direction == 1 || direction == 2)
        QApplication::sendEvent(this, e);
    delete e;
}

/**
 * @brief Controller::getTargetSize retorna el tamaño del vector
 * de targets para que la red neuronal sepa el tamaño y pueda iterar
 * según su tamaño.
 * @return tamaño del vector targetKey.
 */
int
Controller::getTargetSize()
{
#ifdef __DEBUG__
    qDebug("TARGE SIZE: %i", targetKey->length());
#endif
    return targetKey->length();
}

/**
 * @brief Controller::getAtTarget retorna el dato concreto de una
 * posición dentro del vector.
 * @param position, la posición a retornar.
 * @return el dato con indice position.
 */
int
Controller::getAtTarget(int position)
{
    return targetKey->at(position);
}

/**
 * @brief Controller::getInputsSize retorna el tamaño del vector
 * de inputs para que la red neuronal conozca el tamaño y lo pueda
 * iterar.
 * @return tamaño del vector inputs
 */
int
Controller::getInputsSize()
{
    return inputs->length();
}

/**
 * @brief Controller::getAtInputs devuelve el dato con indice position
 * dentro del vector de inputs.
 * @param position, indice del vector.
 * @return dato dentro del vector inputs con indice position.
 */
int
Controller::getAtInputs(int position)
{
    return inputs->at(position);
}

/**
 * @brief Controller::getAtDistance devuelve un dato del vector
 * lasDistance con indice, index.
 * @param index, indice para el vector lastDistance.
 * @return dato dentro del vector lastDistance.
 */
int
Controller::getAtDistance(int index)
{
    return lastDistance.at(index);
}

/**
 * @brief Controller::isPredict retorna el flag 'predict' para que la
 * red sepa si entrenar o predecir resultados sin entrenar.
 * @return predict
 */
int
Controller::isPredict()
{
    return predict;
}

/**
 * @brief Controller::getThreshold retorna el valor de THRESHOLD para que
 * los datos de salida de la red estén por debajo del THRESHOLD.
 * @return THRESHOLD
 */
double
Controller::getThreshold()
{
    return THRESHOLD;
}

/**
 * @brief Controller::getEpochs obtiene el número introducido en el
 * QTextEdit del GUI y lo retorna, para que la red entrene según las
 * epocas que se han introducido.
 * Si no se introduce un número o algo distinto de un número, por defecto
 * son TRAIN_EPOCHS epocas de entrenamiento.
 * @return el total de epocas
 */
int
Controller::getEpochs()
{
    int inputEpochs;
    QString epochs (ui->inputEpoch->toPlainText());
    inputEpochs = epochs.toInt();

    if (!inputEpochs)
        inputEpochs = TRAIN_EPOCHS;

    return inputEpochs;
}

/**
 * @brief Controller::setLabelError añade la interfaz gráfica
 * el error medio de la epoca actual de entrenamiento y las
 * epocas que lleva la red neuronal entrenadas.
 * @param err
 * @param epoch
 */
void
Controller::setLabelError(double err, int epoch)
{
    ui->lblMeanError->setText(QString::number(err));
    ui->lblActEpoch->setText(QString::number(epoch+1));
}

/**********************************************/
/*                                            */
/*      Métodos para la interfaz gráfica      */
/*                                            */
/**********************************************/

/**
 * @brief Controller::setActualDirection muestra la dirección
 * que ha predecido la red neuronal.
 * @param dir
 */
void
Controller::setActualDirection(int dir)
{
    if (dir == 1)
        ui->lblActDirection->setText("LEFT");
    else if (dir == 2)
        ui->lblActDirection->setText("RIGHT");
    else
        ui->lblActDirection->setText("FRONT");
}

/**
 * @brief Controller::changeScoreValue modifica el score
 * actual del juego.
 */
void
Controller::changeScoreValue()
{
    QString score = QString::number(snake->getScore());
    ui->scoreLabelNum->setText(score);
}

/**
 * @brief Controller::debugPosition muestra la posición de la
 * cabeza en todo momento.
 */
void
Controller::debugPosition()
{
    QString x, y;
    x = QString("SNAKE X: ");
    y = QString("SNAKE Y: ");

    x += QString::number(snake->getHeadPosition().x());
    y += QString::number(snake->getHeadPosition().y());


    ui->lblSnakeX->setText(x);
    ui->lblSnakeY->setText(y);
    ui->lblInputsSize->setText(QString::number(inputs->length() / 4));
}

/**
 * @brief Controller::debugFruit muestra la posición de la fruta.
 */
void
Controller::debugFruit()
{
    QString x, y;
    x = QString("FRUIT X: ");
    y = QString("FRUIT Y: ");

    x += QString::number(fruit->x() / BODY_SIZE);
    y += QString::number(fruit->y() / BODY_SIZE);


    ui->lblFruitX->setText(x);
    ui->lblFruitY->setText(y);
}
