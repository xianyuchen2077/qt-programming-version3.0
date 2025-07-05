#ifndef QT_PROGRAMMING_2024_CHARACTER_H
#define QT_PROGRAMMING_2024_CHARACTER_H

#include <QGraphicsEllipseItem>
#include "../HeadEquipments/HeadEquipment.h"
#include "../Armors/Armor.h"
#include "../LegEquipments/LegEquipment.h"
#include "../Weapons/Weapon.h"

class Character : public Item
{
public:
    explicit Character(QGraphicsItem *parent, const QString &pixmapPath);

    [[nodiscard]] bool isLeftDown() const;

    void setLeftDown(bool leftDown);

    [[nodiscard]] bool isRightDown() const;

    void setRightDown(bool rightDown);

    [[nodiscard]] bool isPickDown() const;

    void setPickDown(bool pickDown);

    [[nodiscard]] bool isDownDown() const;

    void setDownDown(bool downDown);

    [[nodiscard]] const QPointF &getVelocity() const;

    void setVelocity(const QPointF &velocity);

    [[nodiscard]] bool isPicking() const;

    void setPicking(bool picking);

    [[nodiscard]] qreal getMoveSpeed() const;

    void setMoveSpeed(qreal speed);

    void processInput();

    // 与血条相关函数
    [[nodiscard]] int getHealth() const; // 获取血量
    void setHealth(int health);          // 设置血量
    void takeDamage(int damage);         // 受伤函数
    void heal(int amount);               // 治疗函数

    Armor* pickupArmor(Armor* newArmor);

protected:
    HeadEquipment *headEquipment{};
    LegEquipment *legEquipment{};
    Armor *armor{};
    Weapon *weapon{};
    QPointF velocity{};

private:
    bool leftDown{}, rightDown{}, pickDown{}, downDown{};
    bool lastPickDown{}; // 上一次拾取键是否被按下
    bool picking{}; // 是否正在拾取物品
    qreal moveSpeed; // 移动速度
    int maxHealth; // 最大血量
    int currentHealth; // 当前血量
    QGraphicsRectItem *healthBarBackground; // 血条背景
    QGraphicsRectItem *healthBarFill; // 血条填充（表示当前血量）

    void drawHealthBar(); // 绘制血条
    void updateHealthBar(); // 更新血条显示
};


#endif //QT_PROGRAMMING_2024_CHARACTER_H
