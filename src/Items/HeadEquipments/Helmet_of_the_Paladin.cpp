#include "Helmet_of_the_Paladin.h"

HelmetOfThePaladin::HelmetOfThePaladin(QGraphicsItem *parent) : HeadEquipment(parent,
                    ":/Items/HeadEquipments/Helmet/Helmet_of_the_Paladin_Icon.png")
{
    // 设置头盔名称和描述
    headequipmentName = "圣骑士的头盔";
    description = "看似是睡衣的帽子，实则是100%免伤的圣骑士头盔";
    durability = 60; // 能够抵挡60滴伤害
    damage_reduction_ratio = 1.0; // 护甲的伤害减免比例为100%
}

void HelmetOfThePaladin::mountToParent()
{
    Mountable::mountToParent();
    setScale(0.30); // 设置缩放比例
    setPos(-3, -32); // 设置位置偏移
}

void HelmetOfThePaladin::unmount()
{
    Mountable::unmount();
    setScale(0.23);
    if (pixmapItem != nullptr)
    {
        qreal pixmapItem_width = pixmapItem->pixmap().width() * pixmapItem->scale();
        qreal pixmapItem_height = pixmapItem->pixmap().height() * pixmapItem->scale();
        pixmapItem->setOffset(-pixmapItem_width/2,- pixmapItem_height);
    }
}

bool HelmetOfThePaladin::takeDamage(int damage, int attackType)
{
    if (durability > 0)
    {
        durability -= damage;
        if (durability < 0)
        {
            durability = 0;
            qDebug() << "MentalArmor durability depleted!";
        }
        qDebug() << "MentalArmor took " << damage << " damage. Remaining durability: " << durability;
        return true;
    }
    else
    {
        qDebug() << "MentalArmor has no durability or is already depleted.";
        return false;
    }
}
