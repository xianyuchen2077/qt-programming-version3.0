#include <QGraphicsScene>
#include <QDebug>
#include <QList>
#include <qtimer.h>
#include "Shabby_Pistol_bullet.h"
#include "../Characters/Character.h"
#include "../Maps/Icefield.h"

BulletBasic::BulletBasic(QGraphicsItem *parent, const QPointF& startPos, const QPointF& direction, int damage)
    : Bullet(parent, BULLET_BASIC_PIXMAP_PATH, startPos, direction, damage)
{
    // 可以调整子弹属性
    bulletSpeed = 20.0; // 提高子弹速度
    lifetimeFrames = 300; // 延长生命周期
}

void BulletBasic::explode()
{
    if (hasExploded) return;

    hasExploded = true;

    // 更换为爆炸图片（如果有的话）
    // 这里可以替换成爆炸效果的图片路径
    // updatePixmap(":/Items/Bullets/explosion.png");

    // 暂时改变颜色表示爆炸
    if (pixmapItem) {
        pixmapItem->setOpacity(0.5);
        setScale(scale() * 2); // 放大表示爆炸
    }

    qDebug() << "Bullet exploded!";

    // // 短暂延迟后销毁
    // QTimer::singleShot(100, this, [this](){
    //     if (scene()) {
    //         scene()->removeItem(this);
    //     }
    //     delete this;
    // });
}

void BulletBasic::handleCollisions() {
    if (hasExploded) return;

    QList<QGraphicsItem*> collidingItems = this->collidingItems();

    for (QGraphicsItem* item : collidingItems) {
        // 检查角色碰撞
        Character* hitCharacter = dynamic_cast<Character*>(item);
        if (hitCharacter) {
            hitCharacter->takeDamage(bulletDamage);
            qDebug() << "BulletBasic hit character! Character health:" << hitCharacter->getHealth();
            explode();
            return;
        }
    }

    // 检查障碍物碰撞
    if (scene()) {
        // 获取场景中的Icefield
        QList<QGraphicsItem*> allItems = scene()->items();
        Icefield* icefield = nullptr;

        for (QGraphicsItem* item : allItems) {
            icefield = dynamic_cast<Icefield*>(item);
            if (icefield) break;
        }

        if (icefield) {
            // 获取障碍物列表
            const QList<Obstacle>& obstacles = icefield->getObstacles();
            QRectF bulletRect = this->sceneBoundingRect();

            for (const Obstacle& obstacle : obstacles) {
                if (obstacle.bounds.intersects(bulletRect)) {
                    qDebug() << "Bullet hit obstacle!";
                    explode();
                    return;
                }
            }
        }
    }
}
