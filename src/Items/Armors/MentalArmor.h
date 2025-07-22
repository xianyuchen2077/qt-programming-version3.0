#ifndef MENTALARMOR_H
#define MENTALARMOR_H


#include "Armor.h"

class MentalArmor : public Armor
{
public:
    explicit MentalArmor(QGraphicsItem *parent = nullptr);

    void mountToParent() override;

    void unmount() override;

    bool takeDamage(int damage, int attackType) override;

    qreal getDamage_reduction_ratio() override { return damage_reduction_ratio; }

    QString getArmorName() const override { return armorName; }
};


#endif // MENTALARMOR_H
