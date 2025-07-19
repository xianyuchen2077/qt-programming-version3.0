#include "Bandage.h"
#include "../Characters/Character.h"
#include <QDebug>

// 图片路径常量定义
const QString Bandage::PIXMAP_PATH = ":/Items/Supplies/Bandage_Icon.png";

Bandage::Bandage(QGraphicsItem *parent)
    : MedicalItem(parent, PIXMAP_PATH)
{
    // 设置绷带的显示属性
    setScale(0.3);

    // 如果图片存在，调整位置使其居中
    if (pixmapItem)
    {
        pixmapItem->setPos(0, -210);
    }

    qDebug() << "Bandage created";
}

void Bandage::mountToParent()
{
    Mountable::mountToParent();
    setScale(0.3);
    if (pixmapItem)
    {
        pixmapItem->setPos(0, -60);  // 设置位置偏移
    }
}

void Bandage::unmount()
{
    Mountable::unmount();
    setScale(0.5);
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPos(0, -90);
    }
}

bool Bandage::ApplytoCharacter(Character* character)
{
    if (!character || isUsed())
    {
        qDebug() << "Cannot use bandage: character is null or item already used";
        return false;
    }

    // 检查角色是否需要治疗
    if (character->getHealth() >= 100)
    {
        qDebug() << "Character already at full health, bandage not needed";
        return false;
    }

    qDebug() << "Using bandage on character with health:" << character->getHealth();

    // 应用立即治疗效果
    applyInstantHeal(character, HEAL_AMOUNT);

    // 标记为已使用并安排删除
    markUsedAndDestroy();

    qDebug() << "Bandage used successfully. Character health now:" << character->getHealth();
    return true;
}
