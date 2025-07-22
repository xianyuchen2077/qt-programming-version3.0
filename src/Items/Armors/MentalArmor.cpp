#include "MentalArmor.h"

MentalArmor::MentalArmor(QGraphicsItem *parent) : Armor(parent, ":/Items/Armors/MentalArmor/MentalArmor.png")
{
    // 设置护甲名称和描述
    armorName = "精神护甲";
    description = "这是一件神秘的金刚护甲，能够抵御半条命攻击，但是只对真（枪）理（械）有效。";
    durability = 50; // 能够抵挡50滴伤害
}

void MentalArmor::mountToParent()
{
    Mountable::mountToParent();
    setScale(0.31);
    setPos(3,-5);
}

void MentalArmor::unmount()
{
    Mountable::unmount();
    setScale(0.4);
    if (pixmapItem != nullptr)
    {
        qreal pixmapItem_width = pixmapItem->pixmap().width() * pixmapItem->scale();
        qreal pixmapItem_height = pixmapItem->pixmap().height() * pixmapItem->scale();
        pixmapItem->setOffset(-pixmapItem_width/2,- pixmapItem_height);
    }
}

void MentalArmor::takeDamage(int damage, int attackType)
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
    } else {
        qDebug() << "MentalArmor has no durability or is already depleted.";
    }
}
