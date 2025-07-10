#include "HeadEquipment.h"

HeadEquipment::HeadEquipment(QGraphicsItem *parent, const QString &pixmapPath) : Item(parent, pixmapPath)
{
    durability = 0; // 初始化耐久度
}

void HeadEquipment::mountToParent()
{
    Mountable::mountToParent();
    setScale(0.4);
    setPos(-30, -210);
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPos(0, 0);
    }
}

void HeadEquipment::unmount()
{
    Mountable::unmount();
    setScale(0.8);
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPos(0, -120);
    }
}

void HeadEquipment::applyEffects(Character* character)
{
    // 如果头部防具有什么特殊的加成，可以在这个调用 character 的函数
    // 例如：在这里添加将 speedBoos 应用到角色属性的逻辑
    // character->addSpeed(speedBoost);
}

void HeadEquipment::removeEffects(Character* character)
{
    // 如果头部防具有什么特殊的加成，可以在这个调用 character 的函数
    // 例如：在这里添加将 speedBoos 从角色属性中移除的逻辑
    // character->removeSpeed(speedBoost);
}

void HeadEquipment::takeDamage(int damage)
{
    if (durability > 0)
    {
        durability -= damage;
        if (durability < 0)
        {
            durability = 0;
            // 可以在这里添加头盔损坏或被摧毁的逻辑
            qDebug() << "HeadEquipment durability depleted!";
        }
        qDebug() << "HeadEquipment took " << damage << " damage. Remaining durability: " << durability;
    } else {
        qDebug() << "HeadEquipment has no durability or is already depleted.";
    }
}

int HeadEquipment::getDurability() const
{
    return durability;
}
