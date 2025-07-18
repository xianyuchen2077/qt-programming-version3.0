#ifndef SOLIDBALL_H
#define SOLIDBALL_H


#include "Weapon.h"

class SolidBall : public Weapon
{
public:
    explicit SolidBall(QGraphicsItem *parent = nullptr);

    void mountToParent() override;
    void unmount() override;

    void shoot(Character* shooter, const QPointF& direction) override;
    Bullet* createBullet(const QPointF& startPos, const QPointF& direction) override;
    Bullet* createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter) override;

private:
    bool Check_and_Destroy() override;
};


#endif // SOLIDBALL_H
