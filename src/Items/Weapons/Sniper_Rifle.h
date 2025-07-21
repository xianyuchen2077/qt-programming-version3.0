#ifndef SNIPER_RIFLE_H
#define SNIPER_RIFLE_H

#include "Weapon.h"
#include <QGraphicsLineItem>

class Sniper_Rifle:public Weapon
{
public:
    explicit Sniper_Rifle(QGraphicsItem *parent=nullptr);
    ~Sniper_Rifle() override;

    void mountToParent() override;
    void unmount() override;

    // 重写射击相关方法
    Bullet* createBullet(const QPointF& startPos, const QPointF& direction) override;
    Bullet* createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter) override;

    // 激光瞄准线功能
    void showLaserSight(const QPointF& startPos, const QPointF& direction);
    void hideLaserSight();
    void updateLaserWithOwner(); // 根据角色位置更新激光瞄准线(外部接口）

    // 激光高度偏移设置
    void setLaserHeightOffset(qreal offset);
    qreal getLaserHeightOffset() const;

private:
    QGraphicsLineItem* laserSight = nullptr;  // 激光瞄准线
    qreal laserHeightOffset = 0.0;          // 激光射出的高度偏移

    QPointF calculateLaserEndPoint(const QPointF& startPos, const QPointF& direction);
    bool checkLineRectIntersection(const QLineF& line, const QRectF& rect);
    QPointF findClosestIntersection(const QLineF& line, const QRectF& rect);
    void updateLaserSight(const QPointF& startPos, const QPointF& direction); // 更新激光瞄准线位置（内部接口）
};


#endif // SNIPER_RIFLE_H
