#include "Icefield.h"
#include <random>

Icefield::Icefield(QGraphicsItem *parent): Map(parent, selectRandomIcefieldPixmapPath()){}

QString Icefield::selectRandomIcefieldPixmapPath()
{
    // 随机数生成器
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(0, 4);
    int randomNumber = distribution(generator);

    // 有20%的概率生成紫色冰原
    if (randomNumber == 0)
    {
        return ":/Items/Maps/Icefield/Icefield_purple.png";
    }
    else
    {
        return ":/Items/Maps/Icefield/Icefield_white.png";
    }
}

qreal Icefield::getFloorHeight()
{
    auto sceneRect = sceneBoundingRect();
    return (sceneRect.top() + sceneRect.bottom()) * 0.63;
}

void Icefield::applyEffectToCharacter(Character *character, qint64 deltaTime)
{
    if (character != nullptr)
    {
        QPointF currentVelocity = character->getVelocity();
        // Reduce speed by 50% on ice
        character->setVelocity(currentVelocity * 1.5);
    }
}
