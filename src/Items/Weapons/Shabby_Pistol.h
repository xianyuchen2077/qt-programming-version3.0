#ifndef SHABBY_PISTOL_H
#define SHABBY_PISTOL_H


#include "Weapon.h"

class Shabby_Pistol: public Weapon
{
public:
    explicit Shabby_Pistol(QGraphicsItem *parent=nullptr);


    void mountToParent() override;
    void unmount() override;

    // 重写射击相关方法
    Bullet* createBullet(const QPointF& startPos, const QPointF& direction) override;
    Bullet* createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter) override;

protected:
    bool Check_and_Destroy() override;
};


#endif // SHABBY_PISTOL_H
