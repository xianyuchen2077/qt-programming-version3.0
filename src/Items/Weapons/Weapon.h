#ifndef WEAPON_H
#define WEAPON_H


#include "../Item.h"
#include "../Mountable.h"

class Weapon : public Item, public Mountable
{
public:
    explicit Weapon(QGraphicsItem *parent, const QString &pixmapPath);

    void mountToParent() override;

    void unmount() override;

    virtual QString getWeaponName() const;
    void setWeaponName(const QString &name);
    virtual QString getDescription() const;
    void setDescription(const QString &desc);
    virtual int getAttackType()const;
    void setAttackType(int type);
    virtual int getAttackElement() const;
    void setAttackElement(int element);
    virtual int getAttackPower() const;
    void setAttackPower(int power);
    virtual int getAttackRange() const;
    void setAttackRange(int range);
    virtual int getAttackSpeed() const;
    void setAttackSpeed(int speed);
    virtual int getCriticalChance() const;
    void setCriticalChance(int chance);
    virtual int getCriticalDamage() const;
    void setCriticalDamage(int damage);
    virtual int getAmmoCount() const;
    void setAmmoCount(int count);
    virtual int getMaxAmmoCount() const;
    void setMaxAmmoCount(int maxCount);
    virtual int getWeight() const;
    void setWeight(int weight);

protected:
    // 武器基本信息
    QString weaponName; // 武器名称
    QString description; // 描述信息

    // 这里可以添加武器特有的属性和方法
    int attackType = 0; // 攻击类型(0：默认近战， 1:近战攻击，2:远程攻击)
    int attackElement = 0; // 攻击元素(0:无元素，1:火焰，2:冰霜，3:雷电，4:毒素等)
    int attackPower = 0; // 攻击力
    int attackRange = 0; // 攻击范围
    int attackSpeed = 0; // 攻击速度
    int criticalChance = 0; // 暴击几率
    int criticalDamage = 0; // 暴击伤害
    int ammoCount = 0; // 弹药数量
    int maxAmmoCount = 0; // 最大弹药数量
    int weight = 0; // 武器重量
};


#endif // WEAPON_H
