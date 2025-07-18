#include "Sniper_Rifle.h"
#include "../Bullets/Sniper_Rifle_bullet.h"
#include "../Characters/Character.h"
#include "../Maps/Icefield.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QPen>

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
    hideLaserSight(); // 卸下时隐藏激光瞄准线
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPos(0, -60);
    }
}

void Sniper_Rifle::showLaserSight(const QPointF& startPos, const QPointF& direction)
{
    if (!scene()) return;

    // 如果激光瞄准线不存在，创建一个
    if (!laserSight)
    {
        laserSight = new QGraphicsLineItem();
        QPen laserPen(Qt::red, 2, Qt::SolidLine);
        laserSight->setPen(laserPen);
        laserSight->setZValue(4.5); // 确保其在子弹之下
        scene()->addItem(laserSight);
    }

    // 计算激光终点
    QPointF endPos = calculateLaserEndPoint(startPos, direction);

    // 设置激光线
    laserSight->setLine(startPos.x(), startPos.y(), endPos.x(), endPos.y());
    laserSight->setVisible(true);
}

void Sniper_Rifle::hideLaserSight()
{
    if (laserSight)
    {
        laserSight->setVisible(false);
    }
}

void Sniper_Rifle::updateLaserSight(const QPointF& startPos, const QPointF& direction)
{
    if (laserSight && laserSight->isVisible())
    {
        QPointF endPos = calculateLaserEndPoint(startPos, direction);
        laserSight->setLine(startPos.x(), startPos.y(), endPos.x(), endPos.y());
    }
}

QPointF Sniper_Rifle::calculateLaserEndPoint(const QPointF& startPos, const QPointF& direction)
{
    if (!scene()) return startPos;

    // 激光的最大射程
    const qreal maxRange = 2000.0;
    QPointF normalizedDirection = direction;

    // 标准化方向向量
    qreal length = qSqrt(normalizedDirection.x() * normalizedDirection.x() + normalizedDirection.y() * normalizedDirection.y());
    if (length > 0)
    {
        normalizedDirection = QPointF(normalizedDirection.x() / length, normalizedDirection.y() / length);
    }

    // 默认终点
    QPointF endPos = startPos + normalizedDirection * maxRange;

    // 查找地图中的障碍物
    QList<QGraphicsItem*> allItems = scene()->items();
    Icefield* icefield = nullptr;

    for (QGraphicsItem* item : allItems)
    {
        icefield = dynamic_cast<Icefield*>(item);
        if (icefield) break;
    }

    if (icefield)
    {
        const QList<Obstacle>& obstacles = icefield->getObstacles();
        qreal minDistance = maxRange;

        // 检查与障碍物的交点
        for (const Obstacle& obstacle : obstacles)
        {
            // 简化的射线-矩形相交检测
            QLineF laserLine(startPos, startPos + normalizedDirection * maxRange);

            // 检查射线与矩形的交点
            QPointF intersectionPoint;
            bool hasIntersection = false;

            // 检查与矩形四条边的交点
            QLineF topEdge(obstacle.bounds.topLeft(), obstacle.bounds.topRight());
            QLineF bottomEdge(obstacle.bounds.bottomLeft(), obstacle.bounds.bottomRight());
            QLineF leftEdge(obstacle.bounds.topLeft(), obstacle.bounds.bottomLeft());
            QLineF rightEdge(obstacle.bounds.topRight(), obstacle.bounds.bottomRight());

            QPointF tempIntersection;
            if (laserLine.intersects(topEdge, &tempIntersection) == QLineF::BoundedIntersection)
            {
                qreal distance = QLineF(startPos, tempIntersection).length();
                if (distance < minDistance)
                {
                    minDistance = distance;
                    intersectionPoint = tempIntersection;
                    hasIntersection = true;
                }
            }
            if (laserLine.intersects(bottomEdge, &tempIntersection) == QLineF::BoundedIntersection)
            {
                qreal distance = QLineF(startPos, tempIntersection).length();
                if (distance < minDistance)
                {
                    minDistance = distance;
                    intersectionPoint = tempIntersection;
                    hasIntersection = true;
                }
            }
            if (laserLine.intersects(leftEdge, &tempIntersection) == QLineF::BoundedIntersection)
            {
                qreal distance = QLineF(startPos, tempIntersection).length();
                if (distance < minDistance)
                {
                    minDistance = distance;
                    intersectionPoint = tempIntersection;
                    hasIntersection = true;
                }
            }
            if (laserLine.intersects(rightEdge, &tempIntersection) == QLineF::BoundedIntersection)
            {
                qreal distance = QLineF(startPos, tempIntersection).length();
                if (distance < minDistance)
                {
                    minDistance = distance;
                    intersectionPoint = tempIntersection;
                    hasIntersection = true;
                }
            }

            if (hasIntersection)
            {
                endPos = intersectionPoint;
            }
        }

        // 检查与地面的交点
        qreal floorHeight = icefield->getFloorHeight();
        if (normalizedDirection.y() > 0) // 向下射击
        {
            qreal distanceToFloor = (floorHeight - startPos.y()) / normalizedDirection.y();
            if (distanceToFloor > 0 && distanceToFloor < minDistance)
            {
                endPos = startPos + normalizedDirection * distanceToFloor;
            }
        }
    }

    return endPos;
}

Bullet* Sniper_Rifle::createBullet(const QPointF& startPos, const QPointF& direction)
{
    return new SniperRifleBullet(nullptr,startPos, direction, nullptr);
}

Bullet* Sniper_Rifle::createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter)
{
    return new SniperRifleBullet(nullptr,startPos, direction, shooter);
}
