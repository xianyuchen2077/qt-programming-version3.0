#include "MentalArmor.h"

MentalArmor::MentalArmor(QGraphicsItem *parent) : Armor(parent, ":/Items/Armors/MentalArmor/MentalArmor.png")
{
    // 设置护甲名称和描述
    armorName = "精神护甲";
    description = "这是一件神秘的金刚护甲，能够抵御半条命攻击。";
    defensePower = 50;
}

void MentalArmor::mountToParent()
{
    Mountable::mountToParent();
    setScale(0.4);
    setPos(0,0);
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
