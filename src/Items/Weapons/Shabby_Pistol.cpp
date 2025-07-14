#include "Shabby_Pistol.h"
#include "../Bullets/Shabby_Pistol_bullet.h"


Shabby_Pistol::Shabby_Pistol(QGraphicsItem *parent) : Weapon(parent, ":/Items/Weapons/Shabby_Pistol_Icon.png")
{
    // 设置武器的初始属性（目前还是纸上谈兵）
    setWeaponName("破旧的手枪"); // 设置武器名称
    setDescription("一把看起来破旧的手枪，虽然外观不佳，但仍然可以发射子弹。"); // 设置武器描述
    attackType=2; // 远程攻击
    attackElement=0; // 无元素攻击
    attackPower=5; // 攻击力
    attackRange=50; // 攻击范围
    attackSpeed=3; // 攻击速度
    criticalChance=10; // 暴击几率
    criticalDamage=15; // 暴击伤害
    ammoCount=6; // 弹药数量
    maxAmmoCount=20; // 最大弹药数量
    weight = 2; // 假设重量为2
    shotCooldown = 300; // 手枪射击间隔300ms
}

void Shabby_Pistol::mountToParent()
{
    Mountable::mountToParent();
    setScale(0.6); // 设置缩放比例
    setPos(7, -13); // 设置位置偏移
}

Bullet* Shabby_Pistol::createBullet(const QPointF& startPos, const QPointF& direction)
{
    return new BulletBasic(nullptr, startPos, direction, attackPower);
}

Bullet* Shabby_Pistol::createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter)
{
    return new BulletBasic(nullptr, startPos, direction, attackPower, shooter);
}
