#include <QTransform>
#include "Character.h"

Character::Character(QGraphicsItem *parent, const QString &pixmapPath) : Item(parent, pixmapPath)
{
    moveSpeed=0.3; // 初始化移动速度

    // 初始化血条
    maxHealth = 100; // 设置最大血量
    currentHealth = maxHealth; // 初始血量等于最大血量
    drawHealthBar(); // 绘制血条
    updateHealthBar(); // 更新血条显示


    leftDown = false;
    rightDown = false;
    pickDown = false;
    downDown = false;
    lastPickDown = false;
    picking = false;
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

// 与速度相关的函数
const QPointF &Character::getVelocity() const {
    return velocity;
}

void Character::setVelocity(const QPointF &velocity) {
    Character::velocity = velocity;
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
        qDebug() << "您的角色寄了!";
        // 触发角色死亡事件，例如从场景中移除角色
        // scene()->removeItem(this); // 如果 Item 继承自 QGraphicsItem 且有场景
    }
    updateHealthBar(); // 更新血条显示
}

// 受伤函数
void Character::takeDamage(int damage)
{
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

    // 按下down键（s）的时候不能进行左右移动
    if (isDownDown())
    {

    }
    else
    {
        if (isLeftDown()) {
            velocity.setX(velocity.x() - moveSpeed);
            setTransform(QTransform().scale(1, 1));  // scale（1，1）代表不进行翻转
        }
        if (isRightDown()) {
            velocity.setX(velocity.x() + moveSpeed);
            setTransform(QTransform().scale(-1, 1));  // scale（-1，1）代表进行水平翻转
        }
    }

    // 更新角色移动速度
    setVelocity(velocity);

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

bool Character::isPicking() const {
    return picking;
}

Armor *Character::pickupArmor(Armor *newArmor) {
    auto oldArmor = armor;
    if (oldArmor != nullptr) {
        oldArmor->unmount();
        oldArmor->setPos(newArmor->pos());
        oldArmor->setParentItem(parentItem());
    }
    newArmor->setParentItem(this);
    newArmor->mountToParent();
    armor = newArmor;
    return oldArmor;
}

