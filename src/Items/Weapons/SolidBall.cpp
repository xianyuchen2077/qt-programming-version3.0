#include "SolidBall.h"
#include "../Bullets/SolidBall_bullet.h"
#include "../Characters/Character.h"
#include <QDebug>
#include <QDateTime>

SolidBall::SolidBall(QGraphicsItem *parent): Weapon(parent, ":/Items/Weapons/SolidBall_Icon.png")
{
    // 设置武器的初始属性
    setWeaponName("吃撑了的实心泰菲");
    setDescription("贪吃的泰菲把自己变成了一个实心球，向前抛出去，砸到就是赚到。");
    setAttackType(2);           // 远程攻击
    setAttackElement(0);        // 无元素攻击
    setAttackPower(20);         // 攻击力
    setAttackRange(50);         // 攻击范围
    setAttackSpeed(3);          // 攻击速度
    setCriticalChance(5);       // 暴击几率
    setCriticalDamage(40);      // 暴击伤害
    setAmmoCount(5);            // 弹药数量
    setMaxAmmoCount(5);         // 最大弹药数量
    setWeight(5);               // 假设重量为5
    setShotCooldown (1000);     // 实心球射击间隔1000ms
}

void SolidBall::mountToParent()
{
    Mountable::mountToParent();
    setScale(0.8);      // 设置缩放比例
    setPos(-78, 0);    // 设置位置偏移
}

void SolidBall::unmount()
{
    Mountable::unmount();
    setScale(1.0);
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPos(0, -80);
    }
}

void SolidBall::shoot(Character* shooter, const QPointF& direction)
{
    if (!canShoot() || !shooter)
    {
        qDebug() << "Cannot throw solid ball: canShoot=" << canShoot() << "shooter=" << (const void*)shooter
                 << "remaining=" << ammoCount;
        return;
    }

    // 计算投掷起始位置
    QPointF shooterPos = shooter->pos();
    QRectF bodyRect = shooter->getBodyCollisionRect();
    QPointF bodyCenter = shooterPos + bodyRect.center();

    // 实心球从角色身体中心稍微向前的位置投掷
    // 根据角色朝向决定偏移方向
    qreal offsetX = shooter->isFaceRight() ? 25 : -25;
    QPointF throwStartPos = bodyCenter + QPointF(offsetX, -20);

    // 创建实心球
    QPointF throwDirection = shooter->isFaceRight() ? QPointF(1, 0) : QPointF(-1, 0);
    SolidBall_Bullet* solidball = new SolidBall_Bullet(nullptr, throwStartPos, throwDirection, getAttackPower(), shooter);

    if (solidball)
    {
        // 将实心球添加到场景
        if (shooter->scene())
        {
            shooter->scene()->addItem(solidball);
            qDebug() << "Solid ball thrown from:" << throwStartPos << "direction:" << throwDirection;
        }
        else
        {
            qDebug() << "Warning: Shooter has no scene!";
            delete solidball;
            return;
        }

        // 减少投掷次数
        ammoCount--;
        lastShotTime = QDateTime::currentMSecsSinceEpoch();

        qDebug() << "Solid ball thrown! Remaining throws:" << ammoCount;

        // 检查是否需要销毁武器
        Check_and_Destroy();
    }
    else
    {
        qDebug() << "Failed to create solid ball!";
    }
}

bool SolidBall::Check_and_Destroy()
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
        return true;
    }
    return false;
}

Bullet* SolidBall::createBullet(const QPointF& startPos, const QPointF& direction)
{
    return new SolidBall_Bullet(nullptr, startPos, direction, attackPower);
}

Bullet* SolidBall::createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter)
{
    return new SolidBall_Bullet(nullptr, startPos, direction, attackPower, shooter);
}
