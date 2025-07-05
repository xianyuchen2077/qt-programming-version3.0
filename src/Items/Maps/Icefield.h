#ifndef ICEFIELD_H
#define ICEFIELD_H


#include "Map.h"

class Icefield: public Map {
public:
    explicit Icefield(QGraphicsItem *parent= nullptr);

    qreal getFloorHeight() override;

    void applyEffectToCharacter(Character* character, qint64 deltaTime) override;

    // 静态方法用于选择随机冰原图片路径
    static QString selectRandomIcefieldPixmapPath();
};


#endif // ICEFIELD_H
