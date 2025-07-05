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
        pixmapItem->setPos(-58, -228); // 人物图片相对与父类 Character 局部原点的位置偏移
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
