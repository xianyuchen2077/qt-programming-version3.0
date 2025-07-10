#include <QGraphicsScene>
#include <QDebug>
#include <QList>
#include "Shabby_Pistol_bullet.h"
#include "../Characters/Character.h"

BulletBasic::BulletBasic(QGraphicsItem *parent, const QPointF& startPos, const QPointF& direction, int damage)
    : Bullet(parent, BULLET_BASIC_PIXMAP_PATH, startPos, direction, damage) // 调用父类构造函数
{
    // 可以设置 BulletBasic 特有的属性或覆盖父类的默认值
    // bulletSpeed = 18.0; // 如果需要比父类默认更快的速度
    // lifetimeFrames = 200; // 如果需要更长的生命周期
}

void BulletBasic::handleCollisions() {
    QList<QGraphicsItem*> collidingItems = this->collidingItems();

    for (QGraphicsItem* item : collidingItems) {
        // 检查是否与 Character 发生碰撞 (假设敌人也是 Character)
        Character* hitCharacter = dynamic_cast<Character*>(item);
        if (hitCharacter) {
            // TODO: 在这里添加逻辑，避免子弹击中发射者自己
            // 例如，如果 Character 有一个唯一的 ID，或者 Bullet 有一个 "owner" 指针
            // if (hitCharacter == this->getOwner()) continue; // 跳过发射者

            hitCharacter->takeDamage(bulletDamage); // 对被击中的角色造成伤害
            qDebug() << "BulletBasic hit character! Character health:" << hitCharacter->getHealth();

            // 子弹击中目标后通常会消失
            if (scene()) {
                scene()->removeItem(this);
            }
            delete this;
            return; // 子弹已销毁，停止遍历
        }
        // 如果子弹需要与墙壁或其他障碍物碰撞后消失，也可以在这里添加逻辑
        // 例如：
        // if (dynamic_cast<Wall*>(item)) {
        //     if (scene()) { scene()->removeItem(this); }
        //     delete this;
        //     return;
        // }
    }
}
