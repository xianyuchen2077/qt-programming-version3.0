#include "Bandage.h"
#include "../Characters/Character.h"
#include <QDebug>

// 图片路径常量定义
const QString Bandage::PIXMAP_PATH = ":/Items/Supplies/Bandage_Icon.png";

Bandage::Bandage(QGraphicsItem *parent)
    : MedicalItem(parent, PIXMAP_PATH)
{
    qDebug() << "Bandage created";
}

void Bandage::mountToParent()
{
    Mountable::mountToParent();
    setScale(0); // 缩放到0使图片消失
}

void Bandage::unmount()
{
    Mountable::unmount();
    setScale(0.3);
    if (pixmapItem != nullptr)
    {
        // 获取缩放后的图片尺寸
        qreal scaledWidth = pixmapItem->boundingRect().width();
        qreal scaledHeight = pixmapItem->boundingRect().height();

        // 计算偏移量，使图片中心与父项的(0,0)对齐
        pixmapItem->setPos(380, scaledHeight + 50); // 已校准！
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
