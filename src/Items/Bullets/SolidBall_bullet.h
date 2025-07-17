#ifndef SOLIDBALL_BULLET_H
#define SOLIDBALL_BULLET_H

#include "Bullet.h"

class Character;

class SolidBall_Bullet: public Bullet
{
public:
    explicit SolidBall_Bullet(QGraphicsItem *parent = nullptr,
                       const QPointF& startPos = QPointF(0,0),
                       const QPointF& direction = QPointF(1,0),
                       int damage = 15,
                       Character* shooter = nullptr);



    void advance(int phase) override;
    int getDamage() const { return ballDamage; }
    void destroyBall();

    // 重写射击相关方法
    // Bullet* createBullet(const QPointF& startPos, const QPointF& direction) override;
    // Bullet* createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter) override;

protected:
    virtual void handleCollisions() override;
    virtual void handleWallCollision();
    virtual void handleGroundCollision();
    virtual void handleCharacterCollision(Character* character);
    virtual void onLanded(); // 落地时调用
    virtual void onDestroy(); // 销毁时调用

private:
    const inline static QString BULLET_BASIC_PIXMAP_PATH = ":/Items/Bullets/SolidBall_bullet_Icon.png";

    QPointF velocity;           // 当前速度向量
    int ballDamage;             // 伤害值
    Character* shooterCharacter; // 发射者

    bool isFlying;              // 是否在飞行中
    bool isLanded;              // 是否已落地
    bool isDestroyed;           // 是否已销毁

    int frameCount;             // 帧计数器
    int landedFrameCount;       // 落地后的帧计数

    static const qreal GRAVITY; // 重力加速度
    static const qreal BOUNCE_FACTOR; // 反弹系数
    static const int DESTROY_DELAY_FRAMES = 180; // 落地后销毁延迟（帧数，60FPS * 2秒）
    static const int SAFETY_FRAMES = 30; // 安全时间（0.5秒内不会击中发射者）
};
#endif // SOLIDBALL_BULLET_H
