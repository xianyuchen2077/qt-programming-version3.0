#include "Weapon.h"
#include "../Characters/Character.h"
#include <QDateTime>
#include <qtimer.h>

Weapon::Weapon(QGraphicsItem *parent, const QString &pixmapPath) : Item(parent, pixmapPath)
{

}

void Weapon::mountToParent()
{
    setScale(0.4);
    setPos(-30, -210);
    Mountable::mountToParent();
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPos(0, 0); // 贴图左上角归位到自身坐标原点
    }
}

void Weapon::unmount()
{
    Mountable::unmount();
    setScale(0.8);
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPos(0, -120);
    }
}

QString Weapon::getWeaponName() const
{
    return weaponName;
}

QString Weapon::getDescription() const
{
    return description;
}

int Weapon::getAttackType() const
{
    return attackType;
}

int Weapon::getAttackElement() const
{
    return attackElement;
}

int Weapon::getAttackPower() const
{
    return attackPower;
}

int Weapon::getAttackRange() const
{
    return attackRange;
}

int Weapon::getAttackSpeed() const
{
    return attackSpeed;
}

int Weapon::getCriticalChance() const
{
    return criticalChance;
}

int Weapon::getCriticalDamage() const
{
    return criticalDamage;
}

int Weapon::getAmmoCount() const
{
    return ammoCount;
}

int Weapon::getMaxAmmoCount() const
{
    return maxAmmoCount;
}

int Weapon::getWeight() const
{
    return weight;
}

int Weapon::getShotCooldown() const
{
    return shotCooldown;
}

void Weapon::setWeaponName(const QString &name)
{
    weaponName = name; // 设置武器名称
}

void Weapon::setDescription(const QString &desc)
{
    description = desc; // 设置武器描述
}

void Weapon::setAttackType(int type)
{
    attackType = type; // 攻击类型(0:默认近战， 1:近战攻击，2:远程攻击)
}

void Weapon::setAttackElement(int element)
{
    attackElement = element; // 攻击元素(0:无元素，1:火焰，2:冰霜，3:雷电，4:毒素等)
}

void Weapon::setAttackPower(int power)
{
    attackPower = power; // 攻击力
}

void Weapon::setAttackRange(int range)
{
    attackRange = range; // 攻击范围
}

void Weapon::setAttackSpeed(int speed)
{
    attackSpeed = speed; // 攻击速度
}

void Weapon::setCriticalChance(int chance)
{
    criticalChance = chance; // 暴击几率
}

void Weapon::setCriticalDamage(int damage)
{
    criticalDamage = damage; // 暴击伤害
}

void Weapon::setAmmoCount(int count)
{
    ammoCount = count; // 弹药数量
}

void Weapon::setMaxAmmoCount(int maxCount)
{
    maxAmmoCount = maxCount; // 最大弹药数量
}

void Weapon::setWeight(int weight)
{
    this->weight = weight; // 武器重量
}

void Weapon::setShotCooldown(int cooldown)
{
    shotCooldown = cooldown; // 设置射击冷却时间
}

// 射击相关方法实现
bool Weapon::canShoot() const
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    return (ammoCount > 0) && (currentTime - lastShotTime >= shotCooldown);
}

Bullet* Weapon::createBullet(const QPointF& startPos, const QPointF& direction)
{
    // 基类默认实现，子类应该重写
    return nullptr;
}

// 添加重载的 createBullet 方法
Bullet* Weapon::createBullet(const QPointF& startPos, const QPointF& direction, Character* shooter)
{
    // 默认实现调用原来的方法（为了兼容性）
    return createBullet(startPos, direction);
}


void Weapon::shoot(Character* shooter, const QPointF& direction)
{
    if (!canShoot() || !shooter)
    {
        return;
    }

    // 计算子弹起始位置（角色中心位置）
    QPointF shooterPos = shooter->pos();

    // 获取角色的身体碰撞框，用于计算中心位置
    QRectF bodyRect = shooter->getBodyCollisionRect();
    QPointF bodyCenter = shooterPos + bodyRect.center();

    // 子弹从角色身体中心稍微往前的位置发射
    QPointF bulletStartPos = bodyCenter + QPointF(direction.x() * 30, 0);

    // 创建子弹（传递shooter参数）
    Bullet* bullet = createBullet(bulletStartPos, direction, shooter);
    if (bullet) {
        // 将子弹添加到场景
        if (shooter->scene()) {
            shooter->scene()->addItem(bullet);
            qDebug() << "Bullet added to scene at:" << bulletStartPos;
        } else {
            qDebug() << "Warning: Shooter has no scene!";
            delete bullet;
            return;
        }

        // 消耗弹药
        ammoCount--;
        lastShotTime = QDateTime::currentMSecsSinceEpoch();

        // 检查是否需要销毁武器
        qDebug() << "Weapon fired! Ammo remaining:" << ammoCount;

        // 使用安全的延迟检查方法
        QTimer::singleShot(0, this, &Weapon::checkAndScheduleDestruction);
    }
    else
    {
        qDebug() << "Failed to create bullet!";
    }
}

void Weapon::checkAndScheduleDestruction()
{
    if (ammoCount <= 0 && !isScheduledForDestruction)
    {
        scheduleDestruction();
    }
}

void Weapon::scheduleDestruction()
{
    if (isScheduledForDestruction)
    {
        return;  // 防止重复调用
    }

    isScheduledForDestruction = true;
    qDebug() << "Weapon exhausted, scheduling safe removal:" << weaponName;

    // 如果武器已装备，需要从角色身上移除
    if (isMounted() && parentItem())
    {
        Character* character = dynamic_cast<Character*>(parentItem());
        if (character)
        {
            character->unequipWeapon();
            qDebug() << "Weapon removed from character";
        }
    }

    // 从场景中移除
    if (scene())
    {
        scene()->removeItem(this);
    }

    // 使用Qt的安全删除方法
    this->deleteLater();
}
