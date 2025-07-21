#ifndef FIST_H
#define FIST_H

#include "Weapon.h"
#include <QTimer>

class Fist : public Weapon
{
    Q_OBJECT

public:
    explicit Fist(QGraphicsItem *parent = nullptr);

    void mountToParent() override;
    void unmount() override;

    // 重写射击方法为近战攻击
    void shoot(Character* attacker, const QPointF& direction) override;
    bool canShoot() const override;

    // 近战武器不创建子弹
    Bullet* createBullet(const QPointF& startPos, const QPointF& direction) override;
    Bullet* createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter) override;

private slots:
    void resetAttackAnimation();

private:
    void performMeleeAttack(Character* attacker, const QPointF& direction);
    void playAttackAnimation();

    bool isAttacking = false;
    QTimer* animationTimer = nullptr;
    QString originalPixmapPath;
    QString attackPixmapPath;
};

#endif // FIST_H
