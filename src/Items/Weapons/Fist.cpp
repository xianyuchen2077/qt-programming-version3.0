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
    setWeight(0);               // 重量
    setShotCooldown(300);       // 拳头攻击间隔

    // 设置图片路径
    originalPixmapPath = ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Fight/0_Reaper_Man_Fight_002.png";
    attackPixmapPath = ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Fight/0_Reaper_Man_Fight_003.png"; // 攻击的时候两张图片交替播放

    // 初始化动画计时器
    animationTimer = new QTimer(this);
    animationTimer->setSingleShot(true);
    connect(animationTimer, &QTimer::timeout, this, &Fist::resetAttackAnimation);
}

void Fist::mountToParent()
{
    Mountable::mountToParent();
    pixmapItem->setScale(0.3);
    pixmapItem->setPos(-130, -225);
}

void Fist::unmount()
{
    Mountable::unmount();
    setScale(0);
    this->deleteLater();
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
    QRectF attackArea(5, -50, 70, 70);
    qDebug() << "Fist attack area:" << attackArea;

    // // ························攻击区域可视化·······················
    // // 创建矩形区域可视化图元
    // QGraphicsRectItem* attackVisual = new QGraphicsRectItem(attackArea);

    // // 设置位置和角度
    // attackVisual->setBrush(QBrush(QColor(255, 0, 0, 100)));  // 红色透明填充
    // attackVisual->setPen(QPen(Qt::red));
    // attackVisual->setPos(attackCenter);
    // attackVisual->setRotation(qRadiansToDegrees(std::atan2(normalizedDir.y(), normalizedDir.x()))); // 方向对齐
    // attacker->scene()->addItem(attackVisual);

    // // 600毫秒后销毁该图形项
    // QTimer::singleShot(600, [attackVisual]() {
    //     if (attackVisual->scene())
    //     {
    //         attackVisual->scene()->removeItem(attackVisual);
    //     }
    //     delete attackVisual;
    // });
    // qDebug() << "Knife attack area:" << attackArea;

    // 查找攻击范围内的所有角色
    QTransform transform;
    transform.translate(attackCenter.x(), attackCenter.y());
    transform.rotateRadians(std::atan2(normalizedDir.y(), normalizedDir.x()));
    QRectF attackGlobalArea = transform.mapRect(attackArea);

    QList<QGraphicsItem*> allItems = attacker->scene()->items();

    for (QGraphicsItem* item : std::as_const(allItems))
    {
        Character* target = dynamic_cast<Character*>(item);
        if (target && target != attacker)
        {
            QRectF targetRect = target->sceneBoundingRect();

            if (attackGlobalArea.intersects(targetRect))
            {
                // 计算伤害（考虑暴击）
                int finalDamage = attackPower;
                if (rand() % 100 < criticalChance)
                {
                    finalDamage = criticalDamage;
                    qDebug() << "Critical hit!";
                }

                // 对目标造成伤害
                target->takeDamage(finalDamage, 1); // 1是近战攻击类型
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
    animationTimer->start(300); // 恢复原图片
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
