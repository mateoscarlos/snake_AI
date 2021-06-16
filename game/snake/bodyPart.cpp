#include "bodyPart.h"

/**
 * @brief BodyPart::BodyPart constructor de cada parte del cuerpo
 * recibe posiciones x e y.
 * @param x
 * @param y
 * @param parent
 */
BodyPart::BodyPart(int x, int y, QGraphicsRectItem *parent) :
    QGraphicsRectItem(0, 0, BODY_SIZE, BODY_SIZE, parent)
{
    setPos(x, y);
    formatBody();
}

BodyPart::~BodyPart() {}

/**
 * @brief BodyPart::formatBody configura el estilo de la parte
 * del cuerpo.
 */
void
BodyPart::formatBody()
{
    setBrush(QBrush(SNAKE_COLOR, Qt::SolidPattern));
    setPen(QPen(Qt::lightGray));
}

/**
 * @brief BodyPart::getPositions retorna la posición de
 * la parte del cuerpo actual de la serpiente.
 * @return
 */
QPoint
BodyPart::getPositions()
{
    QPoint positions;

    positions.setX(x());
    positions.setY(y());

    return positions;
}
