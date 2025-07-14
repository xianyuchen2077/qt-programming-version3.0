#include "Bullet.h"
#include <QGraphicsScene>
#include <QDebug>
#include <QList>
#include <QTransform>

Bullet::Bullet(QGraphicsItem *parent, const QString &pixmapPath, const QPointF& startPos, const QPointF& direction, int damage)
    : Item(parent, pixmapPath), // 将 pixmapPath 传递给 Item 构造函数
    bulletSpeed(15.0), // 默认子弹速度，子类可以覆盖
    bulletDamage(damage), // 子弹伤害
    lifetimeFrames(180), // 子弹存在约 3 秒 (60FPS * 3秒)
    currentFrameCount(0)
{
    setPos(startPos); // 设置子弹的初始位置

    // 根据子弹方向调整其显示方向
    // 如果子弹是向左发射，翻转图片
    if (directionVector.x() < 0)
    {
        setTransform(QTransform().scale(-1, 1));
    }
    setScale(0.05); // 调整子弹图片大小，请根据实际图片大小调整
    setZValue(2); // 确保子弹显示在角色和背景之上
}

void Bullet::destroyBullet()
{
    if (isDestroyed) return;

    isDestroyed = true;
    if (scene()) {
        scene()->removeItem(this);
    }
    delete this; // 使用deleteLater确保安全销毁
}

void Bullet::advance(int phase)
{
    if (phase == 0 && !isDestroyed)
    {
        // 移动子弹
        setPos(pos() + directionVector * bulletSpeed);

        // 处理子弹的生命周期
        currentFrameCount++;
        if (currentFrameCount >= lifetimeFrames)
        {
            destroyBullet();
            return;
        }

        // 调用虚函数处理碰撞
        handleCollisions();

        // 检查是否超出场景边界
        if (scene() && !scene()->sceneRect().intersects(this->sceneBoundingRect()))
        {
            destroyBullet();
            return;
        }
    }
}
