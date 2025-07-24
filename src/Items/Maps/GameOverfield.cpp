#include "GameOverfield.h"

GameOverfield::GameOverfield(QGraphicsItem *parent) : Map(parent, pixmapPath){}

void GameOverfield::setPixmap(const QString &pixmapPath)
{
    Map::setPixmap(pixmapPath);
}

QString GameOverfield::getPixmapPath() const
{
    return pixmapPath;
}
