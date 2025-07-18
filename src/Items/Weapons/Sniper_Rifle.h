#ifndef SNIPER_RIFLE_H
#define SNIPER_RIFLE_H

#include "Weapon.h"
#include <QGraphicsLineItem>

class Sniper_Rifle:public Weapon
{
public:
    explicit Sniper_Rifle(QGraphicsItem *parent=nullptr);

    void mountToParent() override;
    void unmount() override;

    // 重写射击相关方法
    Bullet* createBullet(const QPointF& startPos, const QPointF& direction) override;
    Bullet* createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter) override;

    // 激光瞄准线功能
    void showLaserSight(const QPointF& startPos, const QPointF& direction);
    void hideLaserSight();
    void updateLaserSight(const QPointF& startPos, const QPointF& direction);

private:
    QGraphicsLineItem* laserSight = nullptr;  // 激光瞄准线
    QPointF calculateLaserEndPoint(const QPointF& startPos, const QPointF& direction);
};


#endif // SNIPER_RIFLE_H
