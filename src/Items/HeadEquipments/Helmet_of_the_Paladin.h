#ifndef HELMET_OF_THE_PALADIN_H
#define HELMET_OF_THE_PALADIN_H


#include "HeadEquipment.h"
// 圣骑士的头盔
class HelmetOfThePaladin: public HeadEquipment
{
public:
    explicit HelmetOfThePaladin(QGraphicsItem *parent=nullptr);

    void mountToParent() override;
};


#endif // HELMET_OF_THE_PALADIN_H
