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

    virtual QString getArmorName() const;

protected:
    QString armorName;  // 护甲名称
    QString description; // 护甲描述
    int defensePower;    // 防御力（按照血量衡量）
};


#endif //QT_PROGRAMMING_2024_ARMOR_H
