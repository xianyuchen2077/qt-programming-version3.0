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

// 判断是否正在拾取物品
bool Character::isPicking() const
{
    return picking;
}

void Character::setPicking(bool picking)
{
    Character::picking = picking;
}

// 判断是否正在战斗
bool Character::isFighting() const
{
    return fighting;
}

void Character::setFighting(bool fighting)
{
    Character::fighting = fighting;
}

// 与速度相关的函数
// 获取速度
const QPointF &Character::getVelocity() const {
    return velocity;
}

// 设置速度
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

// 设置下蹲状态
void Character::setCrouching(bool crouching)
{
    if (isCrouching_ != crouching)
    {
        isCrouching_ = crouching;
        updateWeaponPosition(); // 更新武器位置
        qDebug() << "Character crouch status changed to:" << crouching;
    }
}

// 更新武器位置
void Character::updateWeaponPosition()
{
    if (weapon)
    {
        // 如果武器已经装载到角色身上，重新调用mountToParent来更新位置
        weapon->mountToParent();
    }
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
        // 触发角色死亡事件
        setDead(true);
        qDebug() << "Character has died!";
    }
    updateHealthBar(); // 更新血条显示
}

// 受伤函数
void Character::takeDamage(int damage, int attackType)
{
    int remainingDamage = damage;

    // 首先检查是否有佩戴的头部装备，并且它有耐久度
    if (headEquipment != nullptr && headEquipment->getDurability() > 0)
    {
        // 让防具承受一部分伤害
        int absorbedDamage = remainingDamage * headEquipment->getDamage_reduction_ratio(attackType);  // 计算防具理论上要吸收的伤害
        int damageToEquipment = qMin(absorbedDamage, headEquipment->getDurability()); // 计算防具能承受的最大伤害

        if (headEquipment->takeDamage(damageToEquipment, attackType))                           // 调用护甲的takeDamage
        {
        remainingDamage -= damageToEquipment; // 减去防具承受的伤害
        qDebug() << "HeadEquipment absorbed " << damageToEquipment << " damage, remaining damage: " << remainingDamage;
        }

        // 如果防具耐久度耗尽，自动脱下
        if (headEquipment->getDurability() <= 0)
        {
            unequipHeadEquipment(); // 调用新的卸下函数
            qDebug() << "HeadEquipment destroyed and removed";
        }
    }
    // 然后检查是否有佩戴的护甲，并且它有耐久度
    else if (armor != nullptr && armor->getDurability() > 0)
    {
        // 让护甲承受一部分伤害
        int absorbedDamage = remainingDamage * armor->getDamage_reduction_ratio(attackType);  // 计算护甲理论上要吸收的伤害
        int damageToArmor = qMin(absorbedDamage, armor->getDurability());           // 计算护甲能承受的最大伤害
        if (armor->takeDamage(damageToArmor, attackType))                           // 调用护甲的takeDamage
        {
            remainingDamage -= damageToArmor; // 减去护甲承受的伤害
            qDebug() << "Armor absorbed " << damageToArmor << " damage, remaining damage: " << remainingDamage;
        }

        // 如果护甲耐久度耗尽，自动脱下
        if (armor->getDurability() <= 0)
        {
            unequipArmor(); // 调用新的卸下函数
            qDebug() << "Armor destroyed and removed";
        }
    }

    // 剩余的伤害由角色本身承受
    currentHealth -= remainingDamage;
    if (currentHealth <= 0)
    {
        currentHealth = 0;
        setDead(true); // 设置角色为死亡状态
        updateHealthBar(); // 更新血条显示
        qDebug() << "Character died from damage!";
        return;
    }

    setHealth(currentHealth); // 减少血量并更新血条显示
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
    healthBarBackground = new QGraphicsRectItem(-(healthBarWidth/2), -200, healthBarWidth, healthBarHeight, this); // 作为 Character 的子项
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
    if (currentHealth <= 0)
    {
        healthBarFill->setRect(healthBarBackground->rect().x(), healthBarBackground->rect().y(), 0, healthBarBackground->rect().height());
        healthBarFill->setBrush(QBrush(Qt::black)); // 死亡时血条填充为黑色
        return;
    }

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
    else if (healthRatio <= 0.1 && currentHealth > 0)
    {
        healthBarFill->setBrush(QBrush(Qt::darkRed));
    }
    else
    {
        healthBarFill->setBrush(QBrush(Qt::black));
    }
}

void Character::processInput()
{
    // 如果角色死亡，不处理任何输入
    if (isDead())
    {
        return;
    }

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
    // 如果角色死亡，不处理重力
    if (isDead())
    {
        return;
    }

    if (!onGround) // 如果不在地面上
    {
        Velocity_y += gravity * 0.1; // 增加垂直速度
        setPos(pos() + QPointF(0, Velocity_y)); // 更新位置
    }
}

// 处理跳跃
void Character::handleJump()
{
    // 如果角色死亡，不处理重力
    if (isDead())
    {
        return;
    }

    if (onGround)
    {
        Velocity_y = -(2.5 * jumpStrength / 2); // 设置跳跃初速度
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
    // 如果角色死亡，不处理重力
    if (isDead())
    {
        return;
    }

    if (weapon && weapon->canShoot())
    {
        weapon->shoot(this, direction);
        qDebug() << "Character shooting with weapon:" << weapon->getWeaponName();
    }
}

// 判断是否可以射击
bool Character::canShoot() const
{
    // 首先检查weapon是否存在，避免空指针访问
    if (isDead() || !weapon)
    {
        return false;
    }

    // 然后检查武器状态
    if (weapon->getAmmoCount() <= 0 || weapon->getMaxAmmoCount() <= 0)
    {
        return false;
    }

    return weapon->canShoot();
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

// 获取角色图片
bool Character::getPixmapItem() const
{
    if (pixmapItem != nullptr)
    {
        return true;
    }
    else
    {
        qDebug() << "Character pixmapItem is nullptr!";
        return false;
    }
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
        oldArmor->setZValue(GROUND_ITEM_Z);
    }
    newArmor->setParentItem(this);
    newArmor->mountToParent();
    armor = newArmor;

    updateEquipmentLayers();
    qDebug() << "Equipped armor:" << armor->getArmorName();

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
        oldLegEquipment->setZValue(GROUND_ITEM_Z);
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

    updateEquipmentLayers();
    qDebug() << "Equipped head equipment";

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
        oldWeapon->setZValue(GROUND_ITEM_Z);
    }
    newWeapon->setParentItem(this);
    newWeapon->mountToParent();
    weapon = newWeapon;

    updateEquipmentLayers();
    qDebug() << "Equipped weapon equipment";

    return oldWeapon;
}

// 设置拾取状态（MedicalItem）
MedicalItem *Character::pickupMedicalItem(MedicalItem *newMedicalItem)
{
    auto oldMedicalItem = medicalItem;
    if (oldMedicalItem != nullptr)
    {
        oldMedicalItem->unmount();
        oldMedicalItem->deleteLater();
        oldMedicalItem = nullptr;
    }
    if (newMedicalItem)
    {
    newMedicalItem->setParentItem(this);
    newMedicalItem->mountToParent();
    newMedicalItem->ApplytoCharacter(this);
    newMedicalItem->setUsed(true);
    medicalItem = newMedicalItem;
    }

    return newMedicalItem;
}

// 卸下头部装备
void Character::unequipHeadEquipment()
{
    if (headEquipment != nullptr)
    {
        headEquipment->unmount(); // 卸下头部装备
        headEquipment->setParentItem(parentItem()); // 设置父项为角色的父项
        headEquipment->setZValue(GROUND_ITEM_Z); // 设置为地面物品层级
        headEquipment = nullptr; // 清空头部装备指针
        updateEquipmentLayers();
    }
}

// 卸下护甲
void Character::unequipArmor()
{
    if (armor != nullptr)
    {
        armor->unmount(); // 卸下护甲
        armor->setParentItem(parentItem()); // 设置父项为角色的父项
        armor->setZValue(GROUND_ITEM_Z); // 设置为地面物品层级
        armor = nullptr; // 清空头部装备指针
        updateEquipmentLayers();
    }
}

// 卸下腿部装备
void Character::unequipLegEquipment()
{
    if (legEquipment != nullptr)
    {
        legEquipment->unmount(); // 卸下腿部装备
        legEquipment->setParentItem(parentItem()); // 设置父项为角色的父项
        legEquipment->setZValue(GROUND_ITEM_Z); // 设置为地面物品层级
        legEquipment = nullptr; // 清空腿部装备指针
        updateEquipmentLayers();
    }
}

// 卸下武器
void Character::unequipWeapon()
{
    if (weapon != nullptr)
    {
        weapon->unmount(); // 卸下武器
        weapon->setParentItem(parentItem()); // 设置父项为角色的父项
        weapon->setZValue(GROUND_ITEM_Z); // 设置为地面物品层级
        weapon = nullptr; // 清空武器指针
        updateEquipmentLayers();
    }
}

// 卸下所有装备的函数
void Character::removeAllEquipment()
{
    unequipHeadEquipment(); // 卸下头部装备
    unequipArmor(); // 卸下护甲
    unequipLegEquipment(); // 卸下腿部装备
    unequipWeapon(); // 卸下武器
    qDebug() << "All equipment removed from character";
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

void Character::updateEquipmentLayers()
{
    // 设置角色本身的Z值
    setZValue(CHARACTER_BASE_Z);
    qDebug() << "Character Z-value set to:" << CHARACTER_BASE_Z;

    // 更新腿部装备层级（最底层装备）
    if (legEquipment && legEquipment->isMounted()) {
        legEquipment->setZValue(CHARACTER_BASE_Z + LEG_EQUIPMENT_Z_OFFSET);
        qDebug() << "Leg equipment Z-value set to:" << (CHARACTER_BASE_Z + LEG_EQUIPMENT_Z_OFFSET);
    }

    // 更新盔甲层级
    if (armor && armor->isMounted()) {
        armor->setZValue(CHARACTER_BASE_Z + ARMOR_Z_OFFSET);
        qDebug() << "Armor Z-value set to:" << (CHARACTER_BASE_Z + ARMOR_Z_OFFSET);
    }

    // 更新头部装备层级
    if (headEquipment && headEquipment->isMounted()) {
        headEquipment->setZValue(CHARACTER_BASE_Z + HEAD_EQUIPMENT_Z_OFFSET);
        qDebug() << "Head equipment Z-value set to:" << (CHARACTER_BASE_Z + HEAD_EQUIPMENT_Z_OFFSET);
    }

    // 更新武器层级（最高层）
    if (weapon && weapon->isMounted()) {
        weapon->setZValue(CHARACTER_BASE_Z + WEAPON_Z_OFFSET);
        qDebug() << "Weapon Z-value set to:" << (CHARACTER_BASE_Z + WEAPON_Z_OFFSET);
    }
}
