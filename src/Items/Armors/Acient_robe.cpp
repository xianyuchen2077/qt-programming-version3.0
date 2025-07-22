#include "Acient_robe.h"

AcientRobe::AcientRobe(QGraphicsItem *parent) : Armor(parent, ":/Items/Armors/Ancient_robe_Icon.png")
{
    // 设置护甲名称和描述
    armorName = "古老的法袍";
    description = "这是一件古老的法袍，拥有概念神技能，好消息，可以抵挡一次任意攻击；坏消息，只能抵挡一次攻击。";
    durability = 999999; // 抵御伤害无上限
    damage_reduction_ratio = 1.0; // 护甲的伤害减免比例为100%
}

void AcientRobe::mountToParent()
{
    Mountable::mountToParent();
    setScale(0.31);
    setPos(3,-5);
}

void AcientRobe::unmount()
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

bool AcientRobe::takeDamage(int damage, int attackType)
{
    durability = 0; // 护甲只抵挡一次攻击，所以将耐久度设置为0
    qDebug() << "AcientRobe took " << damage << " damage. Remaining durability: " << durability;
    return true;
}
