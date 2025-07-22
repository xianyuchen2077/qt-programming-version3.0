#ifndef QT_PROGRAMMING_2024_ARMOR_H
#define QT_PROGRAMMING_2024_ARMOR_H


#include "../Item.h"
#include "../Mountable.h"

class Armor : public Item, public Mountable
{
public:
    explicit Armor(QGraphicsItem *parent, const QString &pixmapPath);

    void mountToParent() override;

    void unmount() override;

    virtual void applyEffects(class Character* character);  // 应用效果到角色
    virtual void removeEffects(class Character* character); // 从角色移除效果
    virtual bool takeDamage(int damage, int attackType);    // 抵挡伤害（是否成功）
    virtual qreal getDamage_reduction_ratio(int attackType);              // 获取伤害减免比例

    virtual QString getArmorName() const;

    int getDurability() const;

protected:
    QString armorName;  // 护甲名称
    QString description; // 护甲描述
    int durability = 0;    // 防御力（隐藏血条，用于抵挡攻击）
    qreal damage_reduction_ratio = 0; // 护甲的伤害减免比例（0-100）
};


#endif //QT_PROGRAMMING_2024_ARMOR_H
