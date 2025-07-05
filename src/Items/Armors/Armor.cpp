#include "Armor.h"

Armor::Armor(QGraphicsItem *parent, const QString &pixmapPath) : Item(parent, pixmapPath) {}

void Armor::mountToParent()
{
    Mountable::mountToParent();
    // 将护甲的缩放比例设置为 0.8
    setScale(0.8);
    // 将护甲在父坐标系中的位置设置为 (-59, -176)
    setPos(-59, -176);
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPos(0, 0);
    }
}

void Armor::unmount()
{
    Mountable::unmount();
    setScale(0.8);
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPos(0, -120);
    }
}
