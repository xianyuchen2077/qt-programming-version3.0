#include "Weapon.h"
#include "../Characters/Character.h"
#include <QDateTime>

Weapon::Weapon(QGraphicsItem *parent, const QString &pixmapPath) : Item(parent, pixmapPath)
{

}

void Weapon::mountToParent()
{
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

void Weapon::shoot(Character* shooter, const QPointF& direction)
{
    if (!canShoot() || !shooter) {
        return;
    }

    // 计算子弹起始位置（角色位置偏移）
    QPointF shooterPos = shooter->pos();
    QPointF bulletStartPos = shooterPos + QPointF(direction.x() > 0 ? 30 : -30, -20);

    // 创建子弹
    Bullet* bullet = createBullet(bulletStartPos, direction);
    if (bullet) {
        // 将子弹添加到场景
        if (shooter->scene()) {
            shooter->scene()->addItem(bullet);
        }

        // 消耗弹药
        ammoCount--;
        lastShotTime = QDateTime::currentMSecsSinceEpoch();

        // 发出信号
        emit bulletFired(bullet);

        qDebug() << "Weapon fired! Ammo remaining:" << ammoCount;
    }
}

void Weapon::bulletFired(Bullet* bullet)
{
    if (bullet) {
        qDebug() << "Bullet fired from weapon!";
        // 这里可以添加更多逻辑，比如播放音效、特效等
    }
}
