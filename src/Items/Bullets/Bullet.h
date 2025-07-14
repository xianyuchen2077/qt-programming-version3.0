#ifndef BULLET_H
#define BULLET_H


#include "../Item.h"
#include <QPointF>
#include <QGraphicsScene>

class Bullet : public Item
{
public:
    // 构造函数：parent通常是场景, pixmapPath 是指定的图片, startPos是子弹的起始位置, direction是子弹的移动方向（已归一化向量）, damage是伤害
    explicit Bullet(QGraphicsItem *parent, const QString &pixmapPath, const QPointF& startPos, const QPointF& direction, int damage);
    ~Bullet() override = default;

    // 重写 advance 方法来处理子弹的移动和逻辑
    void advance(int phase) override;

    // 获取子弹造成的伤害
    [[nodiscard]] int getDamage() const { return bulletDamage; }

    // 安全销毁自动的方法
    void destroyBullet();

protected:
    qreal bulletSpeed; // 子弹速度
    int bulletDamage; // 子弹伤害
    QPointF directionVector; // 子弹的移动方向向量 (已归一化)
    int lifetimeFrames; // 子弹的生命周期（帧数）
    int currentFrameCount; // 当前已存在的帧数
    bool isDestroyed = false;

    // 纯虚函数：子类必须实现具体的碰撞处理逻辑
    virtual void handleCollisions() = 0;

    // 辅助函数，用于获取子弹的发射者，以避免击中自己
    // 如果子弹有“所有者”概念，可以在构造函数中传入，并在此处使用
    // 例如： Character* owner; // 在Bullet类中添加此成员
    // [[nodiscard]] Character* getOwner() const { return owner; }
};


#endif // BULLET_H
