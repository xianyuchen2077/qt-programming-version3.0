#include "Shabby_Pistol.h"
#include "../Bullets/Shabby_Pistol_bullet.h"
#include "../Characters/Character.h"
#include <QDebug>

Shabby_Pistol::Shabby_Pistol(QGraphicsItem *parent) : Weapon(parent, ":/Items/Weapons/Shabby_Pistol_Icon.png")
{
    // 设置武器的初始属性
    setWeaponName("破旧的手枪"); // 设置武器名称
    setDescription("一把看起来破旧的手枪，虽然外观不佳，但仍然可以发射子弹。"); // 设置武器描述
    setAttackType(2); // 远程攻击
    setAttackElement(0); // 无元素攻击
    setAttackPower(10); // 攻击力
    setAttackRange(50); // 攻击范围
    setAttackSpeed(3); // 攻击速度
    setCriticalChance(1); // 暴击几率
    setCriticalDamage(15); // 暴击伤害
    setAmmoCount(20); // 弹药数量
    setMaxAmmoCount(20); // 最大弹药数量
    setWeight(2); // 假设重量为2
    setShotCooldown (300); // 手枪射击间隔300ms
}

void Shabby_Pistol::mountToParent()
{
    Mountable::mountToParent();
    setScale(0.6);      // 设置缩放比例
    setPos(-42, -13);     // 设置位置偏移
}

void Shabby_Pistol::unmount()
{
    Mountable::unmount();
    setScale(0.7);
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPos(0, -110);
    }
}

void Shabby_Pistol::Check_and_Destroy()
{
    if (ammoCount <= 0)
    {
        qDebug() << "Solid ball weapon exhausted, scheduling removal";

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

        // 安全删除
        delete this;
    }
}

Bullet* Shabby_Pistol::createBullet(const QPointF& startPos, const QPointF& direction)
{
    return new BulletBasic(nullptr, startPos, direction, attackPower);
}

Bullet* Shabby_Pistol::createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter)
{
    return new BulletBasic(nullptr, startPos, direction, attackPower, shooter);
}
