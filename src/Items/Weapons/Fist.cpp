#include "Fist.h"
#include "../Characters/Character.h"
#include <QDebug>
#include <QDateTime>
#include <QGraphicsScene>
#include <QList>
#include <cmath>

Fist::Fist(QGraphicsItem *parent) : Weapon(parent, ":/Items/Weapons/Fist_Icon.png")
{
    // 设置武器的初始属性
    setWeaponName("共产主义的铁拳"); // 设置武器名称
    setDescription("打得一拳开，免得百拳来。最原始但最直接的攻击方式，虽然攻击力不高，但永远不会损坏。"); // 设置武器描述
    setAttackType(1);           // 近战攻击
    setAttackElement(0);        // 无元素攻击
    setAttackPower(5);          // 攻击力
    setAttackRange(30);         // 攻击范围（像素）
    setAttackSpeed(5);          // 攻击速度
    setCriticalChance(2);       // 暴击几率
    setCriticalDamage(8);       // 暴击伤害
    setAmmoCount(99999999);     // 拳头永远不会用完
    setMaxAmmoCount(99999999);  // 最大弹药数量
    setWeight(0);               // 重量为0
    setShotCooldown(500);       // 拳头攻击间隔500ms

    // 设置图片路径
    originalPixmapPath = ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Fight/0_Reaper_Man_Fight_001.png";
    attackPixmapPath = ":/Items/Weapons/Fist_Attack_Icon.png";  // 攻击时的图片

    // 初始化动画计时器
    animationTimer = new QTimer(this);
    animationTimer->setSingleShot(true);
    connect(animationTimer, &QTimer::timeout, this, &Fist::resetAttackAnimation);
}

void Fist::mountToParent()
{
    Mountable::mountToParent();
    setScale(0.4);      // 设置缩放比例
    setPos(-20, -25);   // 设置位置偏移
}

void Fist::unmount()
{
    Mountable::unmount();
    setScale(0);
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPos(0, -90);
    }
}

bool Fist::canShoot() const
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    return !isAttacking && (currentTime - lastShotTime >= shotCooldown);
}

void Fist::shoot(Character* attacker, const QPointF& direction)
{
    if (!canShoot() || !attacker)
    {
        return;
    }

    // 播放攻击动画
    playAttackAnimation();

    // 执行近战攻击
    performMeleeAttack(attacker, direction);

    // 更新最后攻击时间
    lastShotTime = QDateTime::currentMSecsSinceEpoch();
}

void Fist::performMeleeAttack(Character* attacker, const QPointF& direction)
{
    if (!attacker || !attacker->scene())
    {
        return;
    }

    // 获取攻击者位置
    QPointF attackerPos = attacker->pos();
    QRectF attackerRect = attacker->getBodyCollisionRect();
    QPointF attackerCenter = attackerPos + attackerRect.center();

    // 标准化方向向量
    qreal dirLength = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
    QPointF normalizedDir = dirLength > 0 ? QPointF(direction.x() / dirLength, direction.y() / dirLength) : QPointF(1, 0);

    // 计算攻击区域中心
    QPointF attackCenter = attackerCenter + normalizedDir * (attackRange / 2.0);

    // 创建攻击区域（圆形区域）
    QRectF attackArea(attackCenter.x() - attackRange / 2.0,
                      attackCenter.y() - attackRange / 2.0,
                      attackRange, attackRange);

    qDebug() << "Fist attack area:" << attackArea;

    // 查找攻击范围内的所有角色
    QList<QGraphicsItem*> allItems = attacker->scene()->items(attackArea);

    for (QGraphicsItem* item : allItems)
    {
        Character* target = dynamic_cast<Character*>(item);
        if (target && target != attacker)
        {
            // 检查目标是否在攻击范围内
            QPointF targetPos = target->pos();
            QRectF targetRect = target->getBodyCollisionRect();
            QPointF targetCenter = targetPos + targetRect.center();

            qreal distance = std::sqrt(std::pow(targetCenter.x() - attackerCenter.x(), 2) +
                                       std::pow(targetCenter.y() - attackerCenter.y(), 2));

            if (distance <= attackRange)
            {
                // 计算伤害（考虑暴击）
                int finalDamage = attackPower;
                if (rand() % 100 < criticalChance)
                {
                    finalDamage = criticalDamage;
                    qDebug() << "Critical hit!";
                }

                // 对目标造成伤害
                target->takeDamage(finalDamage);
                // qDebug() << "Fist hit" << target->getCharacterName() << "for" << finalDamage << "damage";
            }
        }
    }
}

void Fist::playAttackAnimation()
{
    if (isAttacking) return;

    isAttacking = true;

    // 更换为攻击图片
    if (pixmapItem)
    {
        pixmapItem->setPixmap(QPixmap(attackPixmapPath));
    }

    // 设置动画持续时间
    animationTimer->start(200); // 200ms后恢复原图片
}

void Fist::resetAttackAnimation()
{
    isAttacking = false;

    // 恢复原图片
    if (pixmapItem)
    {
        pixmapItem->setPixmap(QPixmap(originalPixmapPath));
    }
}

// 拳头不创建子弹
Bullet* Fist::createBullet(const QPointF& startPos, const QPointF& direction)
{
    return nullptr;
}

Bullet* Fist::createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter)
{
    return nullptr;
}
