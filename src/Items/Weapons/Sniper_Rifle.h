#ifndef SNIPER_RIFLE_H
#define SNIPER_RIFLE_H

#include "Weapon.h"

class Sniper_Rifle:public Weapon
{
public:
    explicit Sniper_Rifle(QGraphicsItem *parent=nullptr);

    void mountToParent() override;
    void unmount() override;

    // 重写射击相关方法
    Bullet* createBullet(const QPointF& startPos, const QPointF& direction) override;
    Bullet* createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter) override;

protected:
    bool Check_and_Destroy() override;
};


#endif // SNIPER_RIFLE_H
