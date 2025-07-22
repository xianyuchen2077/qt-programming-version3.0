#include "Armor.h"

Armor::Armor(QGraphicsItem *parent, const QString &pixmapPath) : Item(parent, pixmapPath) {}

void Armor::mountToParent()
{
    Mountable::mountToParent();
    // 将护甲的缩放比例设置为 0.8
    setScale(0.8);
    // 将护甲在父坐标系中的位置设置为 (-59, -176)
    setPos(-59, -176);
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPos(0, 0);
    }
}

void Armor::unmount()
{
    Mountable::unmount();
    setScale(0.8);
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPos(0, -120);
    }
}

void Armor::applyEffects(Character* character)
{
    // 如果胸甲有什么特殊的加成，可以在这个调用 character 的函数
    // 例如：在这里添加将 speedBoos 应用到角色属性的逻辑
    // character->addSpeed(speedBoost);
}

void Armor::removeEffects(Character* character)
{
    // 如果胸甲有什么特殊的加成，可以在这个调用 character 的函数
    // 例如：在这里添加将 speedBoos 从角色属性中移除的逻辑
    // character->removeSpeed(speedBoost);
}

void Armor::takeDamage(int damage, int attackType)
{
    if (durability > 0)
    {
        durability -= damage;
        if (durability < 0)
        {
            durability = 0;
            // 可以在这里添加胸甲损坏或被摧毁的逻辑
            qDebug() << "Armor durability depleted!";
        }
        qDebug() << "Armor took " << damage << " damage. Remaining durability: " << durability;
    } else {
        qDebug() << "Armor has no durability or is already depleted.";
    }
}

QString Armor::getArmorName() const
{
    return "Unknown Armor";
}

int Armor::getDurability() const
{
    return durability;
}
