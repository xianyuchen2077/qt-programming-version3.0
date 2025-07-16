#ifndef SOLIDBALL_H
#define SOLIDBALL_H


#include "Weapon.h"

class SolidBall : public Weapon
{
public:
    explicit SolidBall(QGraphicsItem *parent = nullptr);

    void mountToParent() override;
    void unmount() override;

    bool canShoot() const override;
    void shoot(Character* shooter, const QPointF& direction) override;

private:
    void Check_and_Destroy() override;
};


#endif // SOLIDBALL_H
