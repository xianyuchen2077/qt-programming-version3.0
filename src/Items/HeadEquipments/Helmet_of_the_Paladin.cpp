#include "Helmet_of_the_Paladin.h"

HelmetOfThePaladin::HelmetOfThePaladin(QGraphicsItem *parent) : HeadEquipment(parent,
                    ":/Items/HeadEquipments/Helmet/Helmet_of_the_Paladin.png")
{
    durability = 100; // 设置耐久度
}

void HelmetOfThePaladin::mountToParent()
{
    HeadEquipment::mountToParent();
    setScale(1.1); // 设置缩放比例
    setPos(-80, -220); // 设置位置偏移
}
