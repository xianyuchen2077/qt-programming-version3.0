#include "Bullet.h"
#include <QGraphicsScene>
#include <QDebug>
#include <QList>
#include <QTransform>

Bullet::Bullet(QGraphicsItem *parent, const QString &pixmapPath, const QPointF& startPos, const QPointF& direction, int damage)
    : Item(parent, pixmapPath),
    bulletSpeed(15.0),
    bulletDamage(damage),
    lifetimeFrames(300),  // 延长生命周期到5秒 (60FPS * 5秒)
    currentFrameCount(0),
    isDestroyed(false)
{
    setPos(startPos);

    // 标准化方向向量
    qreal length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    if (length > 0)
    {
        directionVector = QPointF(direction.x() / length, direction.y() / length);
    }
    else
    {
        directionVector = QPointF(1, 0); // 默认向右
    }

    // 根据子弹方向调整其显示方向
    if (directionVector.x() < 0)
    {
        setTransform(QTransform().scale(-1, 1));
    }

    setScale(0.5); // 设置子弹缩放比例
    setZValue(5); // 确保子弹显示在最上层

    qDebug() << "Bullet created at:" << startPos << "direction:" << directionVector << "damage:" << damage;
}

void Bullet::destroyBullet()
{
    if (isDestroyed) return;

    isDestroyed = true;
    qDebug() << "Bullet destroyed at position:" << pos();

    if (scene())
    {
        scene()->removeItem(this);
    }
    delete this;
}

void Bullet::advance(int phase)
{
    if (phase == 0 && !isDestroyed)
    {
        // 移动子弹
        QPointF newPos = pos() + directionVector * bulletSpeed;
        setPos(newPos);

        // 处理子弹的生命周期
        currentFrameCount++;
        if (currentFrameCount >= lifetimeFrames)
        {
            qDebug() << "Bullet lifetime expired";
            destroyBullet();
            return;
        }

        // 调用虚函数处理碰撞
        handleCollisions();

        // 检查是否超出场景边界
        if (scene() && !scene()->sceneRect().intersects(this->sceneBoundingRect()))
        {
            qDebug() << "Bullet left scene boundary";
            destroyBullet();
            return;
        }
    }
}
