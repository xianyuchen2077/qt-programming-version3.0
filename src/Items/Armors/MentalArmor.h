#ifndef MENTALARMOR_H
#define MENTALARMOR_H


#include "Armor.h"

class MentalArmor : public Armor
{
public:
    explicit MentalArmor(QGraphicsItem *parent = nullptr);

    void mountToParent() override;

    void unmount() override;

    void takeDamage(int damage, int attackType) override;

    QString getArmorName() const override { return armorName; }
};


#endif // MENTALARMOR_H
