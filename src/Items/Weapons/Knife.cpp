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
    setShotCooldown(800);       // 小刀攻击间隔800ms（比拳头慢一点）

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
    setScale(0.52);
    setPos(45,-80);

    // 设置旋转中心为图片中心（要考虑缩放后的尺寸）
    pixmapItem->setTransformOriginPoint(pixmapItem->boundingRect().width() / 2, pixmapItem->boundingRect().height() / 2);

    // 旋转角度
    pixmapItem->setRotation(-45);  // 顺时针旋转90°

    setZValue(0);
}

void Knife::unmount()
{
    Mountable::unmount();
    pixmapItem->setTransformOriginPoint(pixmapItem->boundingRect().width() / 2, pixmapItem->boundingRect().height() / 2);
    pixmapItem->setRotation(0);
    this->updatePixmap(":/Items/Weapons/CrutchKnife_Icon.png");
    setScale(0.5);
    if (pixmapItem != nullptr)
    {
        qreal pixmapItem_width = pixmapItem->pixmap().width() * pixmapItem->scale();
        qreal pixmapItem_height = pixmapItem->pixmap().height() * pixmapItem->scale();
        pixmapItem->setOffset(-pixmapItem_width/2,- pixmapItem_height);
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

    // 计算攻击区域中心（矩形向前延伸）
    QPointF attackCenter = attackerCenter + normalizedDir * (attackRange / 2.0);

    // 创建攻击区域矩形（宽度为85，高度为85，向前延伸60像素）
    QRectF attackArea(-5, -60, 85, 85);

    // 创建矩形区域可视化图元
    QGraphicsRectItem* attackVisual = new QGraphicsRectItem(attackArea);

    // 设置位置和角度
    attackVisual->setBrush(QBrush(QColor(255, 0, 0, 100)));  // 红色透明填充
    attackVisual->setPen(QPen(Qt::red));
    attackVisual->setPos(attackCenter);                      // 中心点位置
    attackVisual->setRotation(qRadiansToDegrees(std::atan2(normalizedDir.y(), normalizedDir.x()))); // 方向对齐
    attacker->scene()->addItem(attackVisual);

    // 600毫秒后销毁该图形项
    QTimer::singleShot(600, [attackVisual]() {
        if (attackVisual->scene()) {
            attackVisual->scene()->removeItem(attackVisual);
        }
        delete attackVisual;
    });

    qDebug() << "Knife attack area:" << attackArea;

    // 查找攻击范围内的所有角色
    QRectF attackGlobalArea = attackVisual->mapRectToScene(attackVisual->rect());  // 获取攻击区域的世界坐标

    QList<QGraphicsItem*> allItems = attacker->scene()->items();

    for (QGraphicsItem* item : std::as_const(allItems))
    {
        Character* target = dynamic_cast<Character*>(item);
        if (target && target != attacker)
        {
            QRectF targetRect = target->sceneBoundingRect();

            if (attackGlobalArea.intersects(targetRect))
            {
                // 命中，处理伤害
                int finalDamage = attackPower;
                if (rand() % 100 < criticalChance)
                {
                    finalDamage = criticalDamage;
                    qDebug() << "Critical hit!";
                }

                target->takeDamage(finalDamage);
            }
        }
    }
}

void Knife::playAttackAnimation(Character* attacker)
{
    if (isAttacking) return;

    isAttacking = true;

    // 停止并删除旧的动画计时器
    if (animationTimer)
    {
        animationTimer->stop();
        animationTimer->deleteLater();
        animationTimer = nullptr;
    }

    m_attackFrames = {
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_003.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_003.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_003.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_004.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_004.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_004.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_006.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_006.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_007.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_009.png",
        ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Run Slashing/0_Reaper_Man_Run Slashing_011.png",
    };

    m_currentAttackFrame = 0;

    if (attacker && attacker->getPixmapItem())
    {
        attacker->updatePixmap(QString("")); // 设置为空的 QPixmap
        qDebug() << "Character pixmap hidden during attack animation.";
    }

    // 立即显示攻击动画的第一帧
    if (attacker && attacker->getPixmapItem())
    {
        attacker->updatePixmap(m_attackFrames[m_currentAttackFrame]);
        qDebug() << "First attack animation frame set!";
    }
    ++m_currentAttackFrame; // 准备下一帧

    // 计算帧间隔
    int totalDurationMs = 800;
    int frameCount = m_attackFrames.size();
    int frameInterval = totalDurationMs / frameCount;

    animationTimer = new QTimer(this);

    QObject::connect(animationTimer, &QTimer::timeout, this, [this, attacker]() {
        if (m_currentAttackFrame < m_attackFrames.size())
        {
            if (attacker && attacker->getPixmapItem())
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

            if (attacker && attacker->getPixmapItem())
            {
                attacker->updatePixmap(originalPixmapPath); // 恢复到原始图片
                qDebug() << "Attack animation finished, restoring original pixmap.";
            }
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
