#ifndef KNIFE_H
#define KNIFE_H

#include "Weapon.h"
#include <QTimer>

class Knife : public Weapon
{
    Q_OBJECT

public:
    explicit Knife(QGraphicsItem *parent = nullptr);

    void mountToParent() override;
    void unmount() override;

    // 重写射击方法为近战攻击
    void shoot(Character* attacker, const QPointF& direction) override;
    bool canShoot() const override;

    void setAttacker(Character* attacker);

    // 近战武器不创建子弹
    Bullet* createBullet(const QPointF& startPos, const QPointF& direction) override;
    Bullet* createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter) override;

private slots:
    void resetAttackAnimation();

private:
    void performMeleeAttack(Character* attacker, const QPointF& direction);
    void playAttackAnimation(Character* attacker);

    bool isAttacking = false;
    QTimer* animationTimer = nullptr;
    int m_currentAttackFrame = 0;
    QStringList m_attackFrames;
    QString originalPixmapPath;
    QString attackPixmapPath;
    Character* m_attacker;
};

#endif // KNIFE_H
