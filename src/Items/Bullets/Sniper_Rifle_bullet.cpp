#include <QGraphicsScene>
#include <QDebug>
#include <QList>
#include <QTimer>
#include <QGraphicsEffect>
#include "Sniper_Rifle_bullet.h"
#include "../Characters/Character.h"
#include "../Maps/Icefield.h"

SniperRifleBullet::SniperRifleBullet(QGraphicsItem *parent, const QPointF& startPos, const QPointF& direction, Character* shooter)
    : Bullet(parent, BULLET_SNIPER_RIFLE_PIXMAP_PATH, startPos, direction, 50),
      hasExploded(false),
      shooterCharacter(shooter)
{
    bulletDamage = 50;                  // 设置子弹伤害
    bulletSpeed = 30.0;                 // 设置子弹速度
    lifetimeFrames = 500;               // 设置生命周期
    setScale(0.4);                      // 设置子弹缩放比例
    setPos(startPos + QPointF(0,-10));  // 初始位置偏移，避免与角色重叠
    setZValue(5);                       // 确保子弹显示在最上层
    qDebug() << "SniperRifleBullet created with shooter:" << (const void*)shooter << "damage:" << bulletDamage;
}

void SniperRifleBullet::explode()
{
    if (hasExploded) return;

    hasExploded = true;
    qDebug() << "SniperRifleBullet exploded at position:" << pos();

    // 停止子弹移动
    bulletSpeed = 0;
    directionVector = QPointF(0, 0);

    // 创建爆炸视觉效果
    if (pixmapItem)
    {
        // 改变颜色和大小表示爆炸
        pixmapItem->setPixmap(QPixmap(":/Items/Bullets/bullet_fire_Icon2.png")); // 替换为爆炸图片
        setScale(scale() * 1);          // 放大表示爆炸
    }

    // 创建一个动态 QTimer 实例，避免使用 SLOT 宏
    QTimer* timer = new QTimer();
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, [this, timer]() {
        timer->stop();
        destroyBullet();
        timer->deleteLater(); // 清理 timer 避免内存泄漏
        qDebug() << "SniperRifleBullet explosion timer finished, bullet destroyed.";
    });
    timer->start(500);
}

void SniperRifleBullet::handleCollisions()
{
    if (hasExploded || isDestroyed) return;

    QList<QGraphicsItem*> collidingItems = this->collidingItems();

    // 检查角色碰撞
    for (QGraphicsItem* item : std::as_const(collidingItems))
    {
        Character* hitCharacter = dynamic_cast<Character*>(item);
        if (hitCharacter && hitCharacter != shooterCharacter) // 不能击中发射者
        {
            // 计算伤害（包括暴击检测）
            int damage = bulletDamage;
            bool isHeadshot = false;

            // 检查是否击中头部
            QRectF bulletRect = this->sceneBoundingRect();
            QRectF headRect = hitCharacter->getHeadCollisionRect();
            headRect.moveTopLeft(hitCharacter->pos() + headRect.topLeft());

            if (bulletRect.intersects(headRect))
            {
                isHeadshot = true;
                damage = bulletDamage * 2; // 头部伤害翻倍
                qDebug() << "HEADSHOT! Double damage applied:" << damage;
            }
            else
            {
                qDebug() << "Body shot, normal damage:" << damage;
            }

            // 对角色造成伤害
            hitCharacter->takeDamage(bulletDamage);
            explode();
            qDebug() << "SniperRifleBullet hit character! Character health:" << hitCharacter->getHealth();
            return;
        }
    }

    // 检查障碍物碰撞（改进的检测方法）
    if (scene())
    {
        // 获取当前子弹位置的矩形
        QRectF bulletRect = this->sceneBoundingRect();

        // 查找Icefield地图
        QList<QGraphicsItem*> allItems = scene()->items();
        Icefield* icefield = nullptr;

        for (QGraphicsItem* item : std::as_const(allItems))
        {
            icefield = dynamic_cast<Icefield*>(item);
            if (icefield) break;
        }

        if (icefield)
        {
            // 获取障碍物列表
            const QList<Obstacle>& obstacles = icefield->getObstacles();

            for (const Obstacle& obstacle : obstacles)
            {
                if (obstacle.bounds.intersects(bulletRect))
                {
                    qDebug() << "SniperRifleBullet hit obstacle at:" << obstacle.bounds;
                    explode();
                    return;
                }
            }

            // 检查是否撞到地面
            qreal floorHeight = icefield->getFloorHeight();
            if (bulletRect.bottom() >= floorHeight)
            {
                qDebug() << "SniperRifleBullet hit ground at height:" << floorHeight;
                explode();
                return;
            }
        }
    }
}

void SniperRifleBullet::destroyBullet()
{
    if (isDestroyed) return;

    isDestroyed = true;
    qDebug() << "SniperRifleBullet destroyed at position:" << pos();

    // 从场景中移除子弹
    if (scene())
    {
        scene()->removeItem(this);
    }

    // 使用安全删除
    this->deleteLater();
}
