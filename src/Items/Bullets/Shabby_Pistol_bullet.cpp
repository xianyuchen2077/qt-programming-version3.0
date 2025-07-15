#include <QGraphicsScene>
#include <QDebug>
#include <QList>
#include <QTimer>
#include <QGraphicsEffect>
#include "Shabby_Pistol_bullet.h"
#include "../Characters/Character.h"
#include "../Maps/Icefield.h"

BulletBasic::BulletBasic(QGraphicsItem *parent, const QPointF& startPos, const QPointF& direction, int damage, Character* shooter)
    : Bullet(parent, BULLET_BASIC_PIXMAP_PATH, startPos, direction, damage),
    hasExploded(false),
    shooterCharacter(shooter)
{
    bulletSpeed = 20.0;                 // 设置子弹速度
    lifetimeFrames = 300;               // 设置生命周期
    setScale(0.1);                      // 设置子弹缩放比例
    setPos(startPos + QPointF(0,-50));  // 初始位置偏移，避免与角色重叠
    setZValue(5);                       // 确保子弹显示在最上层
    qDebug() << "BulletBasic created with shooter:" << shooter << "damage:" << damage;
}

void BulletBasic::explode()
{
    if (hasExploded) return;

    hasExploded = true;
    qDebug() << "BulletBasic exploded at position:" << pos();

    // 创建爆炸视觉效果
    if (pixmapItem)
    {
        // 改变颜色和大小表示爆炸
        pixmapItem->setOpacity(0.8);    // 设置透明度
        setScale(scale() * 3);          // 放大表示爆炸

        // 添加简单的颜色效果（如果需要）
        // QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect();
        // effect->setColor(Qt::red);
        // pixmapItem->setGraphicsEffect(effect);
    }

    // // 延迟销毁以显示爆炸效果
    // QTimer::singleShot(100, this, [this](){
    //     destroyBullet();
    // });
}

void BulletBasic::handleCollisions()
{
    if (hasExploded || isDestroyed) return;

    QList<QGraphicsItem*> collidingItems = this->collidingItems();

    // 检查角色碰撞
    for (QGraphicsItem* item : collidingItems)
    {
        Character* hitCharacter = dynamic_cast<Character*>(item);
        if (hitCharacter && hitCharacter != shooterCharacter) // 不能击中发射者
        {
            // 对角色造成伤害
            hitCharacter->takeDamage(bulletDamage);
            explode();
            qDebug() << "BulletBasic hit character! Character health:" << hitCharacter->getHealth();
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
                    qDebug() << "Bullet hit obstacle at:" << obstacle.bounds;
                    explode();
                    return;
                }
            }

            // 检查是否撞到地面
            qreal floorHeight = icefield->getFloorHeight();
            if (bulletRect.bottom() >= floorHeight)
            {
                qDebug() << "Bullet hit ground at height:" << floorHeight;
                explode();
                return;
            }
        }
    }
}
