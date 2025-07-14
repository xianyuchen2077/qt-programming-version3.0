#ifndef SHABBY_PISTOL_H
#define SHABBY_PISTOL_H


#include "Weapon.h"

class Shabby_Pistol: public Weapon
{
public:
    explicit Shabby_Pistol(QGraphicsItem *parent=nullptr);


    void mountToParent() override;

    // 重写射击相关方法
    Bullet* createBullet(const QPointF& startPos, const QPointF& direction) override;
    Bullet* createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter) override;
};


#endif // SHABBY_PISTOL_H
