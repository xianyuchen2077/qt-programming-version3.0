#include <memory>
#include "Link.h"
#include <QPixmap>
#include <QDebug>
#include "../HeadEquipments/CapOfTheHero.h"
#include "../Armors/OldShirt.h"
#include "../LegEquipments/WellWornTrousers.h"
#include "../Weapons/Shabby_Pistol.h"

Link::Link(QGraphicsItem *parent): Character(parent, ":/Items/Characters/xiaoxin_superman/stand_middle.png")
{
    if (pixmapItem != nullptr)
    {
        pixmapItem->setScale(0.3); // 只对人物图片进行缩放
        pixmapItem->setPos(-58, -225); // 人物图片相对与父类 Character 局部原点的位置偏移
    }

    headEquipment = new CapOfTheHero(this);
    legEquipment = new WellWornTrousers(this);
    armor = new OldShirt(this);
    weapon = new Shabby_Pistol(this);
    headEquipment->mountToParent();
    legEquipment->mountToParent();
    armor->mountToParent();
    weapon->mountToParent();
}

void Link::setCrouchPixmap()
{
    if (pixmapItem != nullptr)
    {
        updatePixmap(":/Items/Characters/xiaoxin_superman/crouch_middle.png");
        pixmapItem->setScale(0.3);
        pixmapItem->setPos(-58, -180); // 确保图片位置正确
    }
}

void Link::setStandPixmap()
{
    if (pixmapItem != nullptr)
    {
        updatePixmap(":/Items/Characters/xiaoxin_superman/stand_middle.png");
        pixmapItem->setScale(0.3);
        pixmapItem->setPos(-58, -225); // 确保图片位置正确
    }
}

void Link::processInput()
{
    Character::processInput();
    // 处理角色特定的输入逻辑
    if (isLeftDown())
    {
        setVelocity(QPointF(-getMoveSpeed(), 0)); // 向左移动
        setStandPixmap(); // 设置站立图片
    }
    else if (isRightDown())
    {
        setVelocity(QPointF(getMoveSpeed(), 0)); // 向右移动
        setStandPixmap(); // 设置站立图片
    }
    else if (isDownDown())
    {
        setCrouchPixmap(); // 设置下蹲图片
        setVelocity(QPointF(0, 0)); // 停止移动
    }
}
