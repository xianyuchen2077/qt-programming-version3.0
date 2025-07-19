#include "MedicalKit.h"
#include "../Characters/Character.h"
#include <QDebug>

// 图片路径常量定义
const QString MedicalKit::PIXMAP_PATH = ":/Items/Supplies/MedicalKit_Icon.png";

MedicalKit::MedicalKit(QGraphicsItem *parent)
    : MedicalItem(parent, PIXMAP_PATH)
{
    // 设置医疗箱的显示属性
    setScale(0.7);

    // 如果图片存在，调整位置使其居中
    if (pixmapItem)
    {
        pixmapItem->setPos(0, -pixmapItem->boundingRect().height() / 2);
    }

    qDebug() << "MedicalKit created";
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
