#ifndef MENTALARMOR_H
#define MENTALARMOR_H


#include "Armor.h"

class MentalArmor : public Armor
{
public:
    explicit MentalArmor(QGraphicsItem *parent = nullptr);

    void mountToParent() override;

    void unmount() override;

    QString getArmorName() const override { return armorName; }

    qreal getdefencePower() const  { return defensePower; } // 精神护甲防御力为50滴血
};


#endif // MENTALARMOR_H
