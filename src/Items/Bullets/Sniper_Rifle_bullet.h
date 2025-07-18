#ifndef SNIPER_RIFLE_BULLET_H
#define SNIPER_RIFLE_BULLET_H

#include "Bullet.h"

class Character; // 前向声明

class SniperRifleBullet : public Bullet
{
public:
    explicit SniperRifleBullet(QGraphicsItem *parent = nullptr,
                               const QPointF& startPos = QPointF(0,0),
                               const QPointF& direction = QPointF(1,0),
                               Character* shooter = nullptr);
    ~SniperRifleBullet() override = default;

    void destroyBullet() override;

public slots:
    void delayedDestroy() { destroyBullet(); }

protected:
    void handleCollisions() override;

private:
    const inline static QString BULLET_SNIPER_RIFLE_PIXMAP_PATH = ":/Items/Bullets/Sniper_Rifle_bullet_Icon.png";

    // 爆炸效果
    void explode();
    bool hasExploded;

    // 发射者指针，避免击中自己
    Character* shooterCharacter;
};

#endif // SNIPER_RIFLE_BULLET_H
