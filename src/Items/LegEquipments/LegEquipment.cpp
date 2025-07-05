#include "LegEquipment.h"

LegEquipment::LegEquipment(QGraphicsItem *parent, const QString &pixmapPath) : Item(parent, pixmapPath){}

void LegEquipment::mountToParent()
{
    Mountable::mountToParent();
}
