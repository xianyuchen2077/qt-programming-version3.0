#include "WellWornTrousers.h"

WellWornTrousers::WellWornTrousers(QGraphicsItem *parent) : LegEquipment(parent, ":/Items/LegEquipments/WellWornTrousers/BotW_Well-Worn_Trousers_Icon.png")
{
    setScale(0.8); // 设置图片缩放比例
    setPos(-60, -110); // 设置位置
    setZValue(1); // 确保在角色之上显示
}
