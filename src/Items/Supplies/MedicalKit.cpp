#include "MedicalKit.h"
#include "../Characters/Character.h"
#include <QDebug>

// 图片路径常量定义
const QString MedicalKit::PIXMAP_PATH = ":/Items/Supplies/MedicalKit_Icon.png";

MedicalKit::MedicalKit(QGraphicsItem *parent)
    : MedicalItem(parent, PIXMAP_PATH)
{
    qDebug() << "MedicalKit created";
}

void MedicalKit::mountToParent()
{
    Mountable::mountToParent();
    setScale(0); // 缩放到0使图片消失
}

void MedicalKit::unmount()
{
    Mountable::unmount();
    setScale(0.35);
    if (pixmapItem != nullptr)
    {
        qreal pixmapItem_width = pixmapItem->pixmap().width() * pixmapItem->scale();
        qreal pixmapItem_height = pixmapItem->pixmap().height() * pixmapItem->scale();
        pixmapItem->setOffset(-pixmapItem_width/2,- pixmapItem_height);
    }
}

bool MedicalKit::ApplytoCharacter(Character* character)
{
    if (!character || isUsed())
    {
        qDebug() << "Cannot use medical kit: character is null or item already used";
        return false;
    }

    // 检查角色是否需要治疗
    if (character->getHealth() >= 100)
    {
        qDebug() << "Character already at full health, medical kit not needed";
        return false;
    }

    qDebug() << "Using medical kit on character with health:" << character->getHealth();

    // 应用立即治疗效果（直接设置为满血）
    int currentHealth = character->getHealth();
    int healAmount = 100 - currentHealth; // 计算实际需要治疗的量

    applyInstantHeal(character, healAmount);

    // 标记为已使用并安排删除
    markUsedAndDestroy();

    qDebug() << "Medical kit used successfully. Character health now:" << character->getHealth();
    return true;
}
