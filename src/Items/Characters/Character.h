#ifndef QT_PROGRAMMING_2024_CHARACTER_H
#define QT_PROGRAMMING_2024_CHARACTER_H

#include <QGraphicsEllipseItem>
#include "../HeadEquipments/HeadEquipment.h"
#include "../Armors/Armor.h"
#include "../LegEquipments/LegEquipment.h"
#include "../Weapons/Weapon.h"
#include "../Supplies/MedicalSupply.h"

class Character : public Item
{
public:
    explicit Character(QGraphicsItem *parent, const QString &pixmapPath);

    [[nodiscard]] bool getPixmapItem() const;

    void updatePixmap(const QString &pixmapPath);

    [[nodiscard]] bool isLeftDown() const;

    void setLeftDown(bool leftDown);

    [[nodiscard]] bool isRightDown() const;

    void setRightDown(bool rightDown);

    [[nodiscard]] bool isPickDown() const;

    void setPickDown(bool pickDown);

    [[nodiscard]] bool isDownDown() const;

    void setDownDown(bool downDown);

    [[nodiscard]] bool isUpDown() const;

    void setUpDown(bool upDown);

    [[nodiscard]] const QPointF &getVelocity() const;

    void setVelocity(const QPointF &velocity);

    [[nodiscard]] const qreal &getVelocity_y() const;

    void setVelocity_y(const qreal new_velocity_y);

    [[nodiscard]] bool isPicking() const;

    void setPicking(bool picking);

    [[nodiscard]] bool isFighting() const;

    void setFighting(bool fighting);

    [[nodiscard]] qreal getMoveSpeed() const;

    void setMoveSpeed(qreal speed);

    // 下蹲状态管理
    [[nodiscard]] bool isCrouching() const { return isCrouching_; }
    void setCrouching(bool crouching);
    [[nodiscard]] qreal getCrouchOffset() const { return crouchOffset; }

    virtual void processInput();

    // 与血条相关函数
    [[nodiscard]] int getHealth() const;            // 获取血量
    void setHealth(int health);                     // 设置血量
    void takeDamage(int damage, int attackType = 0);    // 受伤函数
    void heal(int amount);                          // 治疗函数

    // 与死亡相关函数
    bool isDead() const { return isDead_; }
    void setDead(bool dead) { isDead_ = dead; }
    bool isDeathAnimationPlaying() const { return isDeathAnimationPlaying_; }
    void setDeathAnimationPlaying(bool playing) { isDeathAnimationPlaying_ = playing; }

    // 与跳跃相关函数
    void handleJump();        // 执行跳跃动作
    void handleGravity();     // 处理重力作用
    bool isOnGround();        // 判断是否在地面上
    void setOnGround(bool onGround);       // 设置是否在地面上
    void setGroundY(qreal groundY);        // 设置地面的Y坐标
    int getGroundY() const;                // 获取地面的Y坐标
    QRectF boundingRect() const override;  // 重写boundingRect函数

    // 射击相关方法
    virtual void shoot(const QPointF& direction);
    virtual bool canShoot() const;

    bool getFaceRight() const { return faceRight; } // 获取角色是否面向右侧
    void setFaceRight(bool right) { faceRight = right; } // 设置角色是否面向右侧
    bool isFaceRight() const { return faceRight; } // 判断角色是否面向右侧

    // 获取复合碰撞矩形
    QRectF getHeadCollisionRect() const;
    QRectF getBodyCollisionRect() const;
    QList<QRectF> getAllCollisionRects() const;

    // 隐身状态管理
    bool isHidden() const { return isHidden_; }
    void setHidden(bool hidden);
    void updateHidingStatus();       // 更新隐身状态

    // 与装备相关函数
    void unequipHeadEquipment();            // 卸下头部装备
    void unequipArmor();                    // 卸下护甲
    void unequipLegEquipment();             // 卸下腿部装备
    void unequipWeapon();                   // 卸下武器
    void removeAllEquipment();              // 卸下所有装备

    Weapon* getWeapon() const { return weapon; }                        // 获取当前装备的武器
    HeadEquipment* getHeadEquipment() const { return headEquipment; }   // 获取当前装备的头部装备
    LegEquipment* getLegEquipment() const { return legEquipment; }      // 获取当前装备的腿部装备
    Armor* getArmor() const { return armor; }                           // 获取当前装备的护甲

    Armor* pickupArmor(Armor* newArmor);
    LegEquipment* pickupLegEquipment(LegEquipment* newLegEquipment);
    HeadEquipment* pickupHeadEquipment(HeadEquipment* newHeadEquipment);
    Weapon* pickupWeapon(Weapon* newWeapon);
    MedicalItem* pickupMedicalItem(MedicalItem* newMedicalItem);

    // 更新装备放置层级
    void updateEquipmentLayers();

protected:
    bool faceRight = false; // 角色是否面向右侧

    HeadEquipment *headEquipment{};
    LegEquipment *legEquipment{};
    Armor *armor{};
    Weapon *weapon{};
    MedicalItem *medicalItem{};

    QPointF velocity{};

    // 角色的碰撞矩形 (局部坐标)
    QRectF m_collisionRect;
    // 角色的碰撞矩形
    QRectF m_headCollisionRect;  // 头部碰撞框
    QRectF m_bodyCollisionRect;  // 身体碰撞框

    // 下蹲状态相关
    bool isCrouching_ = false;      // 是否处于下蹲状态
    qreal crouchOffset = 20.0;      // 下蹲时的偏移量（像素）

private:
    bool leftDown{}, rightDown{}, pickDown{}, downDown{}, upDown{}; // 按键状态;
    bool lastPickDown{}; // 上一次拾取键是否被按下
    bool picking{}; // 是否正在拾取物品
    bool fighting{}; // 是否正在攻击
    qreal moveSpeed; // 移动速度

    // 装备层级（Z值层级定义）
    static constexpr qreal CHARACTER_BASE_Z = 10.0;
    static constexpr qreal LEG_EQUIPMENT_Z_OFFSET = 0.5;   // 腿部装备在角色上方
    static constexpr qreal ARMOR_Z_OFFSET = 1.0;           // 盔甲在腿部装备上方
    static constexpr qreal HEAD_EQUIPMENT_Z_OFFSET = 2.0;  // 头部装备在盔甲上方
    static constexpr qreal WEAPON_Z_OFFSET = 3.0;          // 武器在最上方
    static constexpr qreal GROUND_ITEM_Z = 5.0;            // 地面物品层级

    // 跳跃相关属性
    qreal Velocity_y{}; // 竖直方向的速度
    bool onGround{}; // 是否在地面上
    qreal jumpStrength{}; // 跳跃的初始力量
    qreal gravity{}; // 重力加速度
    qreal groundY{}; // 地面的Y坐标（用于碰撞检测和着地）

    // 血条相关属性
    int maxHealth; // 最大血量
    int currentHealth; // 当前血量
    QGraphicsRectItem *healthBarBackground; // 血条背景
    QGraphicsRectItem *healthBarFill; // 血条填充（表示当前血量）
    void drawHealthBar(); // 绘制血条
    void updateHealthBar(); // 更新血条显示

    // 草地隐身相关属性
    bool isHidden_ = false;          // 是否处于隐身状态
    qreal normalOpacity = 1.0;       // 正常透明度
    qreal hiddenOpacity = 0.01;       // 隐身时的透明度

    // 死亡相关属性
    bool isDead_ = false;                    // 是否死亡
    bool isDeathAnimationPlaying_ = false;   // 死亡动画是否正在播放

    // 更新武器位置的方法
    void updateWeaponPosition();
};


#endif //QT_PROGRAMMING_2024_CHARACTER_H
