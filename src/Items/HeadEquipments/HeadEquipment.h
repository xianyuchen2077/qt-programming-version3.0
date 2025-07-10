#ifndef QT_PROGRAMMING_2024_HEADEQUIPMENT_H
#define QT_PROGRAMMING_2024_HEADEQUIPMENT_H


#include "../Item.h"
#include "../Mountable.h"

class HeadEquipment: public Item, public Mountable
{
public:
    explicit HeadEquipment(QGraphicsItem *parent, const QString &pixmapPath);

    void mountToParent() override;

    void unmount() override;

    virtual void applyEffects(class Character* character); // 应用效果到角色
    virtual void removeEffects(class Character* character); // 从角色移除效果
    virtual void takeDamage(int damage); // 受到伤害

    int getDurability() const;

protected:
    int durability; // 隐藏血条，用于抵挡攻击

};


#endif //QT_PROGRAMMING_2024_HEADEQUIPMENT_H
