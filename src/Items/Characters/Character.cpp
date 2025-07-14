#include <QTransform>
#include <QDebug>
#include "Character.h"

Character::Character(QGraphicsItem *parent, const QString &pixmapPath) : Item(parent, pixmapPath)
{
    moveSpeed=0.3; // 初始化移动速度

    // 初始化血条
    maxHealth = 100; // 设置最大血量
    currentHealth = maxHealth; // 初始血量等于最大血量
    drawHealthBar(); // 绘制血条
    updateHealthBar(); // 更新血条显示

    Velocity_y = 0; // 初始垂直速度为0
    onGround = true; // 初始时在地面上
    jumpStrength = 2; // 跳跃等级，分为 1 2 3 三级，默认为 2 级
    gravity = 1.0; // 重力加速度，单位是 g=9.8m/s²
    groundY = 0; // 初始地面高度为0

    // 初始化按键状态
    leftDown = false;
    rightDown = false;
    pickDown = false;
    downDown = false;
    upDown = false;
    lastPickDown = false;
    picking = false;

    if (pixmapItem)
    {
        // 假设碰撞体是图片中心下方的一部分
        qreal imageWidth = pixmapItem->pixmap().width() * pixmapItem->scale();
        qreal imageHeight = pixmapItem->pixmap().height() * pixmapItem->scale();

        // 头部碰撞框（较宽，在上半部分）
        qreal headWidth = imageWidth * 0.8;
        qreal headHeight = imageHeight * 0.6;
        qreal headX = (imageWidth - headWidth) / 2.0;
        qreal headY = imageHeight * 0.1;

        m_headCollisionRect = QRectF(headX, headY, headWidth, headHeight);

        // 身体碰撞框（较窄，在下半部分）
        qreal bodyWidth = imageWidth * 0.6;  // 身体宽度占60%
        qreal bodyHeight = imageHeight * 0.5; // 身体高度占50%
        qreal bodyX = (imageWidth - bodyWidth) / 2.0;
        qreal bodyY = imageHeight * 0.7; // 从图片50%位置开始

        m_bodyCollisionRect = QRectF(bodyX, bodyY, bodyWidth, bodyHeight-45);
    }
    else
    {
        // 如果没有 pixmapItem，给一个默认值
        m_headCollisionRect = QRectF(0, 0, 50, 80); // 默认尺寸
        m_bodyCollisionRect = QRectF(0, 0, 30, 50); // 默认站立碰撞体尺寸
    }
}

bool Character::isLeftDown() const {
    return leftDown;
}

void Character::setLeftDown(bool leftDown) {
    Character::leftDown = leftDown;
}

bool Character::isRightDown() const {
    return rightDown;
}

void Character::setRightDown(bool rightDown) {
    Character::rightDown = rightDown;
}

bool Character::isPickDown() const {
    return pickDown;
}

void Character::setPickDown(bool pickDown) {
    Character::pickDown = pickDown;
}

bool Character::isDownDown() const {
    return downDown;
}

void Character::setDownDown(bool downDown) {
    Character::downDown = downDown;
}

bool Character::isUpDown() const
{
    return upDown;
}

void Character::setUpDown(bool upDown)
{
    Character::upDown = upDown;
}

// 与速度相关的函数
const QPointF &Character::getVelocity() const {
    return velocity;
}

void Character::setVelocity(const QPointF &velocity) {
    Character::velocity = velocity;
}

// 获取垂直速度
const qreal &Character::getVelocity_y() const
{
    return Velocity_y;
}

// 设置垂直速度
void Character::setVelocity_y(const qreal new_velocity_y)
{
    Velocity_y = new_velocity_y;
}

// 获取移动速度
qreal Character::getMoveSpeed() const
{
    return moveSpeed;
}

// 设置移动速度
void Character::setMoveSpeed(qreal speed)
{
    moveSpeed = speed;
}

// 获取血量
int Character::getHealth() const
{
    return currentHealth;
}

// 设置血量
void Character::setHealth(int health)
{
    currentHealth = qBound(0, health, maxHealth); // 确保血量在 0 和 maxHealth 之间
    if (currentHealth <= 0)
    {
        // 触发角色死亡事件，例如从场景中移除角色
        // scene()->removeItem(this); // 如果 Item 继承自 QGraphicsItem 且有场景
    }
    updateHealthBar(); // 更新血条显示
}

// 受伤函数
void Character::takeDamage(int damage)
{
    int remainingDamage = damage;

    // 首先检查是否有佩戴的头部装备，并且它有耐久度
    if (headEquipment != nullptr && headEquipment->getDurability() > 0)
    {
        // 让防具承受一部分伤害
        int damageToEquipment = qMin(remainingDamage, headEquipment->getDurability()); // 计算防具能承受的最大伤害
        headEquipment->takeDamage(damageToEquipment); // 调用防具的takeDamage
        remainingDamage -= damageToEquipment; // 减去防具承受的伤害

        // 如果防具耐久度耗尽，自动脱下
        if (headEquipment->getDurability() <= 0)
        {
            unequipHeadEquipment(); // 调用新的卸下函数
        }
    }

    // 剩余的伤害由角色本身承受
    currentHealth -= remainingDamage;
    if (currentHealth <= 0)
    {
        currentHealth = 0;
        // 可以在这里添加角色死亡的逻辑
    }

    setHealth(currentHealth - damage); // 减少血量
}

// 治疗函数
void Character::heal(int amount)
{
    setHealth(currentHealth + amount); // 增加血量
}

// 绘制血条
void Character::drawHealthBar()
{
    // (X_relative, Y_relative, width, height, parent)
    int healthBarWidth = 80; // 血条宽度
    int healthBarHeight = 10; // 血条高度
    healthBarBackground = new QGraphicsRectItem(-(healthBarWidth/2), -250, healthBarWidth, healthBarHeight, this); // 作为 Character 的子项
    healthBarBackground->setBrush(QBrush(Qt::darkGray)); // 设置背景颜色
    healthBarBackground->setPen(QPen(Qt::black,5)); // 有边框, 边框颜色为黑色，宽度为5个像素
    healthBarBackground->setZValue(2); // 确保在角色之上显示

    // 血条填充 (绿色或红色)
    healthBarFill = new QGraphicsRectItem(-(healthBarWidth/2), -250, healthBarWidth, healthBarHeight, this); // 作为 Character 的子项
    healthBarFill->setBrush(QBrush(Qt::green)); // 初始填充颜色
    healthBarFill->setPen(QPen(Qt::NoPen)); // 无边框
    healthBarFill->setZValue(3); // 在背景之上，确保可见
}

// 更新血条显示
void Character::updateHealthBar()
{
    qreal healthRatio = (qreal)currentHealth / maxHealth;
    qreal fillWidth = healthRatio * healthBarBackground->rect().width();
    healthBarFill->setRect(healthBarBackground->rect().x(), healthBarBackground->rect().y(), fillWidth, healthBarBackground->rect().height());

    // 根据血量百分比改变血条颜色
    if (healthRatio > 0.5)
    {
        healthBarFill->setBrush(QBrush(Qt::green));
    }
    else if (healthRatio > 0.2)
    {
        healthBarFill->setBrush(QBrush(Qt::yellow));
    }
    else if (healthRatio > 0.1)
    {
        healthBarFill->setBrush(QBrush(Qt::red));
    }
    else
    {
        healthBarFill->setBrush(QBrush(Qt::black));
    }
}

void Character::processInput()
{
    auto velocity = QPointF(0, 0);

    if (!lastPickDown && pickDown)
    {
        picking = true;
    }
    else
    {
        picking = false;
    }
    // 更新拾取键状态
    lastPickDown = pickDown;
}

// 处理重力
void Character::handleGravity()
{
    if (!onGround) // 如果不在地面上
    {
        Velocity_y += gravity * 0.1; // 增加垂直速度
        setPos(pos() + QPointF(0, Velocity_y)); // 更新位置
    }
}

// 处理跳跃
void Character::handleJump()
{
    if (onGround)
    {
        Velocity_y = -(2.25 * jumpStrength / 2); // 设置跳跃初速度
        // Velocity_y = -(3 * jumpStrength / 2); // 备选跳跃初速度
        onGround = false; // 跳跃后不在地面上
    }
}

// 判断是否在地面上
bool Character::isOnGround()
{
    return this->onGround;
}

// 设置是否在地面上
void Character::setOnGround(bool onGround)
{
    this->onGround = onGround;
}

// 设置地面的Y坐标
void Character::setGroundY(qreal groundY)
{
    this->groundY = groundY;
}

// 获取地面的Y坐标
int Character::getGroundY() const
{
    return groundY;
}

// 角色射击函数
void Character::shoot(const QPointF& direction)
{
    if (weapon && weapon->canShoot()) {
        weapon->shoot(this, direction);
    }
}

// 判断是否可以射击
bool Character::canShoot() const
{
    return weapon && weapon->canShoot();
}

// 判断是否正在拾取物品
bool Character::isPicking() const
{
    return picking;
}

// 设置拾取状态(Armor)
Armor *Character::pickupArmor(Armor *newArmor)
{
    auto oldArmor = armor;
    if (oldArmor != nullptr)
    {
        oldArmor->unmount();
        oldArmor->setPos(newArmor->pos());
        oldArmor->setParentItem(parentItem());
    }
    newArmor->setParentItem(this);
    newArmor->mountToParent();
    armor = newArmor;
    return oldArmor;
}

// 设置拾取状态(LegEquipment)
LegEquipment *Character::pickupLegEquipment(LegEquipment *newLegEquipment)
{
    auto oldLegEquipment = legEquipment;
    if (oldLegEquipment != nullptr)
    {
        oldLegEquipment->unmount();
        oldLegEquipment->setPos(newLegEquipment->pos());
        oldLegEquipment->setParentItem(parentItem());
    }
    newLegEquipment->setParentItem(this);
    newLegEquipment->mountToParent();
    legEquipment = newLegEquipment;
    return oldLegEquipment;
}

// 设置拾取状态(HeadEquipment)
HeadEquipment *Character::pickupHeadEquipment(HeadEquipment *newHeadEquipment)
{
    auto oldHeadEquipment = headEquipment;
    if (oldHeadEquipment != nullptr)
    {
        oldHeadEquipment->unmount();
        oldHeadEquipment->setPos(newHeadEquipment->pos());
        oldHeadEquipment->setParentItem(parentItem());
    }
    newHeadEquipment->setParentItem(this);
    newHeadEquipment->mountToParent();
    headEquipment = newHeadEquipment;
    return oldHeadEquipment;
}

// 设置拾取状态(Weapon)
Weapon *Character::pickupWeapon(Weapon *newWeapon)
{
    auto oldWeapon = weapon;
    if (oldWeapon != nullptr)
    {
        oldWeapon->unmount();
        oldWeapon->setPos(newWeapon->pos());
        oldWeapon->setParentItem(parentItem());
    }
    newWeapon->setParentItem(this);
    newWeapon->mountToParent();
    weapon = newWeapon;
    return oldWeapon;
}

// 更换角色图片
void Character::updatePixmap(const QString &pixmapPath)
{
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPixmap(QPixmap(pixmapPath));
    }
    else
    {
        qDebug() << "Character is nullptr!";
    }
}

// 佩戴头部装备
void Character::equipHeadEquipment(HeadEquipment *newHeadEquipment)
{
    if (headEquipment != nullptr)
    {
        HeadEquipment *oldHeadEquipment = headEquipment; // 保存当前装备
        headEquipment->unmount(); // 卸下当前装备

        // 将当前装备位置和父项设置为新装备的位置和角色的父项
        headEquipment->setPos(newHeadEquipment->pos()); // 设置位置为新装备的位置
        headEquipment->setParentItem(parentItem()); // 设置父项为角色的父项
    }
    newHeadEquipment->setParentItem(this); // 设置新装备的父项为角色
    newHeadEquipment->mountToParent(); // 挂载到角色上
    headEquipment = newHeadEquipment; // 更新角色的头部装备
}

// 卸下头部装备
void Character::unequipHeadEquipment()
{
    if (headEquipment != nullptr)
    {
        headEquipment->unmount(); // 卸下头部装备
        headEquipment->setParentItem(parentItem()); // 设置父项为角色的父项
        headEquipment = nullptr; // 清空头部装备指针
    }
}

// 重写boundingRect函数
QRectF Character::boundingRect() const
{
    return m_headCollisionRect.united(m_bodyCollisionRect);
}

QRectF Character::getHeadCollisionRect() const
{
    return m_headCollisionRect;
}

QRectF Character::getBodyCollisionRect() const
{
    return m_bodyCollisionRect;
}

QList<QRectF> Character::getAllCollisionRects() const
{
    return {m_headCollisionRect, m_bodyCollisionRect};
}
