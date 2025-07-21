#include "Knife.h"
#include "../Characters/Character.h"
#include <QDebug>
#include <QDateTime>
#include <QGraphicsScene>
#include <QList>
#include <cmath>

Knife::Knife(QGraphicsItem *parent) : Weapon(parent, ":/Items/Weapons/CrutchKnife_Icon.png")
{
    // 设置武器的初始属性
    setWeaponName("大巫师之杖"); // 设置武器名称
    setDescription("这虽然是一个法杖，但奈何你是一个麻瓜，只能够进行物理攻击"); // 设置武器描述
    setAttackType(1);           // 近战攻击
    setAttackElement(0);        // 无元素攻击
    setAttackPower(15);         // 攻击力
    setAttackRange(50);         // 攻击范围（比拳头大）
    setAttackSpeed(4);          // 攻击速度
    setCriticalChance(8);       // 暴击几率（比拳头高）
    setCriticalDamage(25);      // 暴击伤害
    setAmmoCount(999999);       // 小刀也不会用完（耐久度很高）
    setMaxAmmoCount(999999);    // 最大弹药数量
    setWeight(1);               // 重量为1
    setShotCooldown(600);       // 小刀攻击间隔600ms（比拳头慢一点）

    // 设置图片路径
    originalPixmapPath = ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_011.png";
    attackPixmapPath = ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_011.png";  // 攻击时的图片

    // 初始化动画计时器
    animationTimer = new QTimer(this);
    animationTimer->setSingleShot(true);
    connect(animationTimer, &QTimer::timeout,this, &Knife::resetAttackAnimation);
}

void Knife::mountToParent()
{
    Mountable::mountToParent();
    setScale(0.26);        // 设置缩放比例
    setPos(-165,-115);

    // 设置旋转中心为图片中心（要考虑缩放后的尺寸）
    pixmapItem->setTransformOriginPoint(pixmapItem->boundingRect().width() / 2, pixmapItem->boundingRect().height() / 2);

    // 旋转角度
    pixmapItem->setRotation(-45);  // 顺时针旋转90°

    setZValue(0);
}

void Knife::unmount()
{
    Mountable::unmount();
    setScale(0.3);
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPos(600, 150);
    }
}

bool Knife::canShoot() const
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    return !isAttacking && (currentTime - lastShotTime >= shotCooldown);
}

void Knife::shoot(Character* attacker, const QPointF& direction)
{
    if (!canShoot() || !attacker)
    {
        return;
    }

    // qDebug() << "Knife attack initiated by" << attacker->getCharacterName();

    // 播放攻击动画
    playAttackAnimation(attacker);

    // 执行近战攻击
    performMeleeAttack(attacker, direction);

    // 更新最后攻击时间
    lastShotTime = QDateTime::currentMSecsSinceEpoch();
}

void Knife::performMeleeAttack(Character* attacker, const QPointF& direction)
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

    // 计算攻击区域中心（小刀的攻击范围更大）
    QPointF attackCenter = attackerCenter + normalizedDir * (attackRange / 2.0);

    // 创建攻击区域（椭圆形区域，向前延伸）
    QRectF attackArea(attackCenter.x() - attackRange,
                      attackCenter.y() - attackRange,
                      attackRange * 2.0 / 3.0, attackRange);

    // 绘制攻击区域
    // 在 performMeleeAttack 内部（计算完 attackArea 后）添加：
    QGraphicsEllipseItem* attackVisual = new QGraphicsEllipseItem(attackArea);
    attackVisual->setBrush(QBrush(QColor(255, 0, 0, 100)));  // 红色透明填充
    attackVisual->setPen(QPen(Qt::red));                     // 红色边框
    QPointF center = attackArea.center();
    attackVisual->setTransformOriginPoint(center); // 设置旋转原点
    attackVisual->setRotation(45);                 // 顺时针旋转 45°
    // 添加到 scene 中
    attacker->scene()->addItem(attackVisual);


    qDebug() << "Knife attack area:" << attackArea;

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
                // 小刀还需要检查攻击方向（只能攻击前方的敌人）
                QPointF toTarget = targetCenter - attackerCenter;
                qreal dotProduct = normalizedDir.x() * toTarget.x() + normalizedDir.y() * toTarget.y();

                if (dotProduct > 0) // 目标在攻击者前方
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
                    // qDebug() << "Knife hit" << target->getCharacterName() << "for" << finalDamage << "damage";
                }
            }
        }
    }
}

void Knife::playAttackAnimation(Character* attacker)
{
    if (isAttacking) return;

    isAttacking = true;


    if (animationTimer)
    {
        animationTimer->stop();
        animationTimer->deleteLater();
        animationTimer = nullptr;
    }

    m_currentAttackFrame = 0;
    m_attackFrames = {
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_002.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_003.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_004.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_005.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_006.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_007.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_008.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_009.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_010.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_011.png",
    };

    m_currentAttackFrame = 0;

    // 计算帧间隔（1500ms / 15帧 ≈ 100ms）
    int totalDurationMs = 3000;
    int frameCount = m_attackFrames.size();
    int frameInterval = totalDurationMs / frameCount;

    animationTimer = new QTimer(this);

    QObject::connect(animationTimer, &QTimer::timeout, this, [this, attacker]() {
        if (m_currentAttackFrame < m_attackFrames.size())
        {
            if (pixmapItem)
            {
                attacker->updatePixmap(m_attackFrames[m_currentAttackFrame]);
                qDebug() << "Animation frame" << m_currentAttackFrame << "set!";
            }
            ++m_currentAttackFrame;
        }
        else
        {
            // 播放完毕，清理定时器
            animationTimer->stop();
            animationTimer->deleteLater();
            animationTimer = nullptr;

            isAttacking = false;
        }
    });

    animationTimer->start(frameInterval);  // 启动播放
}

void Knife::resetAttackAnimation()
{
    isAttacking = false;
}

void Knife::setAttacker(Character* attacker)
{
    m_attacker = attacker;
}

// 小刀不创建子弹
Bullet* Knife::createBullet(const QPointF& startPos, const QPointF& direction)
{
    return nullptr;
}

Bullet* Knife::createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter)
{
    return nullptr;
}
