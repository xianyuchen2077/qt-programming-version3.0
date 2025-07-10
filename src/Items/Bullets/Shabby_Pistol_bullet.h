#ifndef SHABBY_PISTOL_BULLET_H
#define SHABBY_PISTOL_BULLET_H


#include "Bullet.h"

class BulletBasic : public Bullet
{
    Q_OBJECT

public:
    explicit BulletBasic(QGraphicsItem *parent = nullptr, const QPointF& startPos = QPointF(0,0), const QPointF& direction = QPointF(1,0), int damage = 10);
    ~BulletBasic() override = default;

protected:
    // 实现父类的纯虚函数
    void handleCollisions() override;

private:
    const inline static QString BULLET_BASIC_PIXMAP_PATH = ":/Items/Bullets/Shabby_Pistol_bullet.png";
};


#endif // SHABBY_PISTOL_BULLET_H
