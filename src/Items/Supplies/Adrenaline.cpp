#include "Adrenaline.h"
#include "../Characters/Character.h"
#include <QDebug>

// 常量定义
const qreal Adrenaline::SPEED_BOOST = 2;
const QString Adrenaline::PIXMAP_PATH = ":/Items/Supplies/Adrenaline_Icon.png";

Adrenaline::Adrenaline(QGraphicsItem *parent)
    : MedicalItem(parent, PIXMAP_PATH)
{
    qDebug() << "Adrenaline created";
}

void Adrenaline::mountToParent()
{
    Mountable::mountToParent();
    setScale(0); // 缩放到0使图片消失
}

void Adrenaline::unmount()
{
    Mountable::unmount();
    setScale(0.6);
    if (pixmapItem != nullptr)
    {
        qreal pixmapItem_width = pixmapItem->pixmap().width() * pixmapItem->scale();
        qreal pixmapItem_height = pixmapItem->pixmap().height() * pixmapItem->scale();
        pixmapItem->setOffset(-pixmapItem_width/2,- pixmapItem_height);
    }
}

bool Adrenaline::ApplytoCharacter(Character* character)
{
    if (!character || isUsed())
    {
        qDebug() << "Cannot use adrenaline: character is null or item already used";
        return false;
    }

    qDebug() << "Using adrenaline on character with health:" << character->getHealth();
    qDebug() << "Character current speed:" << character->getMoveSpeed();

    // 1. 应用立即治疗效果
    if (character->getHealth() < 100)
    {
        applyInstantHeal(character, INSTANT_HEAL);
        qDebug() << "Applied instant heal:" << INSTANT_HEAL;
    }

    // 2. 应用持续治疗效果
    if (character->getHealth() < 100)
    {
        applyContinuousHeal(character, CONTINUOUS_HEAL, HEAL_DURATION);
        qDebug() << "Applied continuous heal:" << CONTINUOUS_HEAL << "over" << HEAL_DURATION << "ms";
    }

    // 3. 应用移动速度加成
    applySpeedBoost(character, SPEED_BOOST, SPEED_DURATION);
    qDebug() << "Applied speed boost:" << SPEED_BOOST << "for" << SPEED_DURATION << "ms";

    // 标记为已使用并安排删除
    markUsedAndDestroy();

    qDebug() << "Adrenaline used successfully";
    qDebug() << "Character health after instant heal:" << character->getHealth();
    qDebug() << "Character speed after boost:" << character->getMoveSpeed();

    return true;
}

