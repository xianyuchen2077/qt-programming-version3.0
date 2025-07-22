#ifndef ACIENT_ROBE_H
#define ACIENT_ROBE_H


#include "Armor.h"

class AcientRobe : public Armor
{
public:
    explicit AcientRobe(QGraphicsItem *parent = nullptr);

    void mountToParent() override;

    void unmount() override;

    bool takeDamage(int damage, int attackType) override;

    qreal getDamage_reduction_ratio(int attackType) override { return damage_reduction_ratio; }

    QString getArmorName() const override { return armorName; }
};


#endif // ACIENT_ROBE_H
