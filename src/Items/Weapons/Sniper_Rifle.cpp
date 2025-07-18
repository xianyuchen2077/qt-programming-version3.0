#include "Sniper_Rifle.h"
#include "../Bullets/Sniper_Rifle_bullet.h"
#include "../Characters/Character.h"
#include <QDebug>

Sniper_Rifle::Sniper_Rifle(QGraphicsItem *parent) : Weapon(parent, ":/Items/Weapons/Sniper_Rifle_Icon.png")
{
    // 设置武器的初始属性
    setWeaponName("龙息之怒狙击枪"); // 设置武器名称
    setDescription("传说中的黑龙化身成为手中的狙击枪，其强大的力量在等待它的主人"); // 设置武器描述
    setAttackType(2);           // 远程攻击
    setAttackElement(0);        // 无元素攻击
    setAttackPower(50);         // 攻击力
    setAttackRange(200);        // 攻击范围
    setAttackSpeed(1);          // 攻击速度
    setCriticalChance(5);       // 暴击几率
    setCriticalDamage(100);     // 暴击伤害
    setAmmoCount(3);           // 弹药数量
    setMaxAmmoCount(3);        // 最大弹药数量
    setWeight(5);               // 假设重量为5
    setShotCooldown (5000);     // 狙击步枪射击间隔5000ms
}

void Sniper_Rifle::mountToParent()
{
    Mountable::mountToParent();
    setScale(0.8);      // 设置缩放比例
    setPos(-42, -17);   // 设置位置偏移
}

void Sniper_Rifle::unmount()
{
    Mountable::unmount();
    setScale(0.8);
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPos(0, -60);
    }
}

void Sniper_Rifle::Check_and_Destroy()
{
    if (ammoCount <= 0)
    {
        qDebug() << "Sniper Rifle weapon exhausted, scheduling removal";

        // 如果武器已装备，需要从角色身上移除
        if (isMounted() && parentItem())
        {
            Character* character = dynamic_cast<Character*>(parentItem());
            if (character)
            {
                character->unequipWeapon();
                qDebug() << "Weapon removed from character";
            }
        }

        // 从场景中移除
        if (scene())
        {
            scene()->removeItem(this);
        }
        delete this; // 删除武器对象
    }
}

Bullet* Sniper_Rifle::createBullet(const QPointF& startPos, const QPointF& direction)
{
    return new SniperRifleBullet(nullptr,startPos, direction, nullptr);
}

Bullet* Sniper_Rifle::createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter)
{
    return new SniperRifleBullet(nullptr,startPos, direction, shooter);
}
