#include "Sniper_Rifle.h"
#include "../Bullets/Sniper_Rifle_bullet.h"
#include "../Characters/Character.h"
#include "../Maps/Icefield.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QPen>
#include <QGraphicsItem>

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

Sniper_Rifle::~Sniper_Rifle()
{
    // 清理激光瞄准线
    if (laserSight && laserSight->scene())
    {
        laserSight->scene()->removeItem(laserSight);
        delete laserSight;
        laserSight = nullptr;
    }
}

void Sniper_Rifle::mountToParent()
{
    Mountable::mountToParent();
    setScale(0.8);      // 设置缩放比例
    setPos(8, -17);   // 设置位置偏移

    // 获取父项（角色）
    Character* ownerCharacter = dynamic_cast<Character*>(parentItem());
    if (ownerCharacter)
    {
        QPointF startPosition = ownerCharacter->pos(); // 获取角色中心位置

        // 根据角色朝向确定激光方向
        QPointF laserDirection;
        if (ownerCharacter->isFaceRight())
        {
            laserDirection = QPointF(1, 0); // 朝右
        }
        else
        {
            laserDirection = QPointF(-1, 0); // 朝左
        }

        // 调整起始位置，使其看起来从武器“枪管”发出
        if (ownerCharacter->isFaceRight())
        {
            startPosition += QPointF(0, laserHeightOffset); // 右侧朝向角色的示例偏移
        } else {
            startPosition += QPointF(-0, laserHeightOffset); // 左侧朝向角色的示例偏移
        }


        showLaserSight(startPosition, laserDirection); // 挂载时显示激光瞄准线
    }
    else
    {
        qDebug() << "Warning: Sniper_Rifle parent is not a Character.";
    }
}

void Sniper_Rifle::unmount()
{
    Mountable::unmount();
    hideLaserSight(); // 卸下时隐藏激光瞄准线
    setScale(0.8);
    if (pixmapItem != nullptr)
    {
        qreal pixmapItem_width = pixmapItem->pixmap().width() * pixmapItem->scale();
        qreal pixmapItem_height = pixmapItem->pixmap().height() * pixmapItem->scale();
        pixmapItem->setOffset(-pixmapItem_width/2,- pixmapItem_height);
    }
}

void Sniper_Rifle::showLaserSight(const QPointF& startPos, const QPointF& direction)
{
    // 检查场景是否存在
    QGraphicsScene* currentScene = scene();
    if (!currentScene)
    {
        // 尝试从父项获取场景
        if (parentItem())
        {
            currentScene = parentItem()->scene();
        }
    }

    if (!currentScene)
    {
        qDebug() << "Warning: No scene available for laser sight";
        return;
    }

    // 如果激光瞄准线不存在，创建一个
    if (!laserSight)
    {
        laserSight = new QGraphicsLineItem();
        QPen laserPen(Qt::red, 3, Qt::SolidLine); // 增加线宽度，使其更可见
        laserSight->setPen(laserPen);
        laserSight->setZValue(4.5); // 确保其在子弹之下但在地图之上
        currentScene->addItem(laserSight);
        qDebug() << "Laser sight created and added to scene";
    }
    else if (laserSight->scene() != currentScene)
    {
        // 如果激光瞄准线在不同的场景中，移动它
        if (laserSight->scene())
        {
            laserSight->scene()->removeItem(laserSight);
        }
        currentScene->addItem(laserSight);
        qDebug() << "Laser sight moved to current scene";
    }

    // 计算激光终点
    QPointF endPos = calculateLaserEndPoint(startPos, direction);

    // 设置激光线
    laserSight->setLine(startPos.x(), startPos.y(), endPos.x(), endPos.y());
    laserSight->setVisible(true);

    qDebug() << "Laser sight updated: from" << startPos << "to" << endPos;
}

void Sniper_Rifle::hideLaserSight()
{
    if (laserSight)
    {
        laserSight->setVisible(false);
        qDebug() << "Laser sight hidden";
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
    QGraphicsScene* currentScene = scene();
    if (!currentScene && parentItem())
    {
        currentScene = parentItem()->scene();
    }

    if (!currentScene)
    {
        // 如果没有场景，返回一个默认的终点
        return startPos + direction * 500.0;
    }

    // 激光的最大射程
    const qreal maxRange = 2000.0;
    QPointF normalizedDirection = direction;

    // 标准化方向向量
    qreal length = qSqrt(normalizedDirection.x() * normalizedDirection.x() + normalizedDirection.y() * normalizedDirection.y());
    if (length > 0)
    {
        normalizedDirection = QPointF(normalizedDirection.x() / length, normalizedDirection.y() / length);
    }
    else
    {
        normalizedDirection = QPointF(1, 0); // 默认向右
    }

    // 默认终点
    QPointF endPos = startPos + normalizedDirection * maxRange;
    qreal minDistance = maxRange;

    // 查找场景中的所有物品
    QList<QGraphicsItem*> allItems = currentScene->items();
    Icefield* icefield = nullptr;
    Character* ownerCharacter = dynamic_cast<Character*>(parentItem());

    // 创建激光射线
    QLineF laserLine(startPos, startPos + normalizedDirection * maxRange);

    for (QGraphicsItem* item : allItems)
    {
        Character* character = dynamic_cast<Character*>(item);

        // 跳过自己（武器的拥有者）
        if (character && character != ownerCharacter && !character->isDead())
        {
            // 获取角色的全局位置
            QPointF characterGlobalPos = character->pos();

            // 获取角色的碰撞框
            QList<QRectF> collisionRects = character->getAllCollisionRects();

            for (const QRectF& rect : collisionRects)
            {
                // 将碰撞框转换到全局坐标系
                QRectF globalRect = rect.translated(characterGlobalPos);

                // 检查激光是否与角色碰撞框相交
                if (checkLineRectIntersection(laserLine, globalRect))
                {
                    // 找到最近的交点
                    QPointF intersection = findClosestIntersection(laserLine, globalRect);
                    qreal distance = QLineF(startPos, intersection).length();

                    if (distance < minDistance && distance > 1.0)
                    {
                        minDistance = distance;
                        endPos = intersection;
                        qDebug() << "Laser hit character at:" << endPos << "distance:" << distance;
                    }
                }
            }
        }
    }

    // 查找地图障碍物
    for (QGraphicsItem* item : allItems)
    {
        icefield = dynamic_cast<Icefield*>(item);
        if (icefield) break;
    }

    if (icefield)
    {
        const QList<Obstacle>& obstacles = icefield->getObstacles();

        // 检查与障碍物的交点
        for (const Obstacle& obstacle : obstacles)
        {
            // 检查与矩形四条边的交点
            QLineF topEdge(obstacle.bounds.topLeft(), obstacle.bounds.topRight());
            QLineF bottomEdge(obstacle.bounds.bottomLeft(), obstacle.bounds.bottomRight());
            QLineF leftEdge(obstacle.bounds.topLeft(), obstacle.bounds.bottomLeft());
            QLineF rightEdge(obstacle.bounds.topRight(), obstacle.bounds.bottomRight());

            QPointF tempIntersection;
            QLineF::IntersectType intersectionType;

            // 检查所有边的交点
            QList<QLineF> edges = {topEdge, bottomEdge, leftEdge, rightEdge};
            for (const QLineF& edge : edges)
            {
                intersectionType = laserLine.intersects(edge, &tempIntersection);
                if (intersectionType == QLineF::BoundedIntersection)
                {
                    qreal distance = QLineF(startPos, tempIntersection).length();
                    if (distance < minDistance && distance > 1.0) // 避免太近的交点
                    {
                        minDistance = distance;
                        endPos = tempIntersection;
                    }
                }
            }
        }

        // 检查与地面的交点
        qreal floorHeight = icefield->getFloorHeight();
        if (normalizedDirection.y() > 0 && startPos.y() < floorHeight) // 向下射击且在地面上方
        {
            qreal distanceToFloor = (floorHeight - startPos.y()) / normalizedDirection.y();
            if (distanceToFloor > 0 && distanceToFloor < minDistance)
            {
                endPos = startPos + normalizedDirection * distanceToFloor;
                qDebug() << "Laser hit floor at:" << endPos;
            }
        }
    }
    else
    {
        // 如果没有找到地图，使用场景边界
        QRectF sceneRect = currentScene->sceneRect();

        // 检查与场景边界的交点
        QLineF topBoundary(sceneRect.topLeft(), sceneRect.topRight());
        QLineF bottomBoundary(sceneRect.bottomLeft(), sceneRect.bottomRight());
        QLineF leftBoundary(sceneRect.topLeft(), sceneRect.bottomLeft());
        QLineF rightBoundary(sceneRect.topRight(), sceneRect.bottomRight());

        QPointF tempIntersection;
        QList<QLineF> boundaries = {topBoundary, bottomBoundary, leftBoundary, rightBoundary};

        for (const QLineF& boundary : boundaries)
        {
            if (laserLine.intersects(boundary, &tempIntersection) == QLineF::BoundedIntersection)
            {
                qreal distance = QLineF(startPos, tempIntersection).length();
                if (distance < minDistance && distance > 1.0)
                {
                    minDistance = distance;
                    endPos = tempIntersection;
                }
            }
        }
    }

    return endPos;
}

bool Sniper_Rifle::checkLineRectIntersection(const QLineF& line, const QRectF& rect)
{
    // 检查线段与矩形四条边是否有交点
    QLineF topEdge(rect.topLeft(), rect.topRight());
    QLineF bottomEdge(rect.bottomLeft(), rect.bottomRight());
    QLineF leftEdge(rect.topLeft(), rect.bottomLeft());
    QLineF rightEdge(rect.topRight(), rect.bottomRight());

    QPointF intersection;

    return (line.intersects(topEdge, &intersection) == QLineF::BoundedIntersection ||
            line.intersects(bottomEdge, &intersection) == QLineF::BoundedIntersection ||
            line.intersects(leftEdge, &intersection) == QLineF::BoundedIntersection ||
            line.intersects(rightEdge, &intersection) == QLineF::BoundedIntersection);
}

QPointF Sniper_Rifle::findClosestIntersection(const QLineF& line, const QRectF& rect)
{
    QList<QLineF> edges = {
        QLineF(rect.topLeft(), rect.topRight()),       // 上边
        QLineF(rect.bottomLeft(), rect.bottomRight()), // 下边
        QLineF(rect.topLeft(), rect.bottomLeft()),     // 左边
        QLineF(rect.topRight(), rect.bottomRight())    // 右边
    };

    QPointF closestIntersection = line.p2(); // 默认返回线段终点
    qreal minDistance = QLineF(line.p1(), line.p2()).length();

    for (const QLineF& edge : edges)
    {
        QPointF intersection;
        if (line.intersects(edge, &intersection) == QLineF::BoundedIntersection)
        {
            qreal distance = QLineF(line.p1(), intersection).length();
            if (distance < minDistance)
            {
                minDistance = distance;
                closestIntersection = intersection;
            }
        }
    }

    return closestIntersection;
}

void Sniper_Rifle::setLaserHeightOffset(qreal offset)
{
    laserHeightOffset = offset;

    // 如果武器已经挂载，更新激光瞄准线
    if (isMounted())
    {
        Character* ownerCharacter = dynamic_cast<Character*>(parentItem());
        if (ownerCharacter)
        {
            QPointF startPosition = ownerCharacter->pos();
            QPointF laserDirection;

            if (ownerCharacter->isFaceRight())
            {
                laserDirection = QPointF(1, 0);
                startPosition += QPointF(40, laserHeightOffset);
            } else {
                laserDirection = QPointF(-1, 0);
                startPosition += QPointF(-40, laserHeightOffset);
            }

            updateLaserSight(startPosition, laserDirection);
        }
    }
}

qreal Sniper_Rifle::getLaserHeightOffset() const
{
    return laserHeightOffset;
}

void Sniper_Rifle::updateLaserWithOwner()
{
    if (!isMounted()) return;

    Character* owner = dynamic_cast<Character*>(parentItem());
    if (!owner) return;

    QPointF shooterPos = owner->pos();
    QRectF bodyRect = owner->getBodyCollisionRect();
    QPointF bodyCenter = shooterPos + bodyRect.center();
    QPointF gunPos = bodyCenter + QPointF(owner->isFaceRight() ? 57 : -57, laserHeightOffset);
    QPointF direction = owner->isFaceRight() ? QPointF(1, 0) : QPointF(-1, 0);

    updateLaserSight(gunPos, direction);
}

Bullet* Sniper_Rifle::createBullet(const QPointF& startPos, const QPointF& direction)
{
    return new SniperRifleBullet(nullptr,startPos, direction, nullptr);
}

Bullet* Sniper_Rifle::createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter)
{
    return new SniperRifleBullet(nullptr,startPos, direction, shooter);
}
