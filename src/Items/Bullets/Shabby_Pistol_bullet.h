#ifndef SHABBY_PISTOL_BULLET_H
#define SHABBY_PISTOL_BULLET_H

#include "Bullet.h"

class Character; // 前向声明

class BulletBasic : public Bullet
{
public:
    explicit BulletBasic(QGraphicsItem *parent = nullptr,
                         const QPointF& startPos = QPointF(0,0),
                         const QPointF& direction = QPointF(1,0),
                         int damage = 10,
                         Character* shooter = nullptr);
    ~BulletBasic() override = default;

    void destroyBullet() override;

public slots:
    void delayedDestroy() { destroyBullet(); }


protected:
    void handleCollisions() override;

private:
    const inline static QString BULLET_BASIC_PIXMAP_PATH = ":/Items/Bullets/Shabby_Pistol_bullet.png";

    // 爆炸效果
    void explode();
    bool hasExploded;

    // 发射者指针，避免击中自己
    Character* shooterCharacter;
};

#endif // SHABBY_PISTOL_BULLET_H
