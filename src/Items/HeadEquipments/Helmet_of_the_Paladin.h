#ifndef HELMET_OF_THE_PALADIN_H
#define HELMET_OF_THE_PALADIN_H


#include "HeadEquipment.h"
// 圣骑士的头盔
class HelmetOfThePaladin: public HeadEquipment
{
public:
    explicit HelmetOfThePaladin(QGraphicsItem *parent=nullptr);

    void mountToParent() override;

    void unmount() override;

    bool takeDamage(int damage, int attackType) override;

    qreal getDamage_reduction_ratio(int attackType) override { return damage_reduction_ratio; }

    QString getHeadEquipmentName() const override { return headequipmentName; }
};


#endif // HELMET_OF_THE_PALADIN_H
