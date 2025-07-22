#ifndef QIN_CEREMONIAL_ROBE_H
#define QIN_CEREMONIAL_ROBE_H


#include "Armor.h"

class Robe_of_Qin : public Armor
{
public:
    explicit Robe_of_Qin(QGraphicsItem *parent = nullptr);

    void mountToParent() override;

    void unmount() override;

    bool takeDamage(int damage, int attackType) override;

    qreal getDamage_reduction_ratio(int attackType) override;

    QString getArmorName() const override { return armorName; }
};


#endif // QIN_CEREMONIAL_ROBE_H
