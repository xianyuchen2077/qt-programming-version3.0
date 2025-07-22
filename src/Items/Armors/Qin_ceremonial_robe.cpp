#include "Qin_ceremonial_robe.h"

Robe_of_Qin::Robe_of_Qin(QGraphicsItem *parent) : Armor(parent, ":/Items/Armors/Robe_of_Qin_Icon.png")
{
    // 设置护甲名称和描述
    armorName = "秦王的礼袍";
    description = "这是秦王当年绕柱躲荆轲时穿的礼袍，有buff加成，拳头和匕首（法杖）的伤害都会很大程度上减免，但是毕竟是冷兵器时代的产物，抗不了热兵器的伤害。";
    durability = 99999999; // 抵御伤害无上限
    damage_reduction_ratio = 1.0; // 护甲的伤害减免比例为100%
}

void Robe_of_Qin::mountToParent()
{
    Mountable::mountToParent();
    setScale(0.31);
    setPos(3,-5);
}

void Robe_of_Qin::unmount()
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

bool Robe_of_Qin::takeDamage(int damage, int attackType)
{
    if (attackType/10 != 1) // 只对近战攻击有效（attackType的第一位是1）
    {
        qDebug() << "Robe_of_Qin only protects against gunfire attacks.";
        return false;
    }

    if (durability > 0)
    {
        durability -= damage;
        if (durability < 0)
        {
            durability = 0;
            qDebug() << "Robe_of_Qin durability depleted!";
        }
        qDebug() << "Robe_of_Qin took " << damage << " damage. Remaining durability: " << durability;
        return true;
    }
    else
    {
        qDebug() << "MentalArmor has no durability or is already depleted.";
        return false;
    }
}

qreal Robe_of_Qin::getDamage_reduction_ratio(int attackType)
{
    if (attackType == 11)
    {
        damage_reduction_ratio = 1.0; // 100%减伤
    }
    else if (attackType == 12)
    {
        damage_reduction_ratio = 0.9; // 90%减伤;
    }
    return damage_reduction_ratio;
}
