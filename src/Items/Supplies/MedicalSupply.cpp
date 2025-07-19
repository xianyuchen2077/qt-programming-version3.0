#include "MedicalSupply.h"
#include "../Characters/Character.h"
#include <QDebug>

MedicalItem::MedicalItem(QGraphicsItem *parent, const QString &pixmapPath)
    : Item(parent, pixmapPath)
    , used(false)
    , continuousHealTimer(nullptr)
    , speedBoostTimer(nullptr)
    , cleanupTimer(nullptr)
    , healTarget(nullptr)
    , remainingHeal(0)
    , healPerTick(0)
    , healTickInterval(1000)
    , speedTarget(nullptr)
    , originalSpeed(0.0)
    , boostMultiplier(1.0){}

MedicalItem::~MedicalItem()
{
    // 清理定时器
    if (continuousHealTimer)
    {
        continuousHealTimer->stop();
        delete continuousHealTimer;
    }
    if (speedBoostTimer)
    {
        speedBoostTimer->stop();
        delete speedBoostTimer;
    }
    if (cleanupTimer)
    {
        cleanupTimer->stop();
        delete cleanupTimer;
    }
}

void MedicalItem::applyInstantHeal(Character* character, int healAmount)
{
    if (!character) return;

    qDebug() << "Applying instant heal:" << healAmount << "to character";
    character->heal(healAmount);

    // 显示治疗效果（可以添加视觉效果）
    qDebug() << "Character health after healing:" << character->getHealth();
}

void MedicalItem::applyContinuousHeal(Character* character, int healAmount, int duration)
{
    if (!character || healAmount <= 0 || duration <= 0) return;

    healTarget = character;
    remainingHeal = healAmount;
    healTickInterval = 1000; // 每秒治疗一次

    // 计算每次治疗量（确保总量正确）
    int totalTicks = duration / healTickInterval;
    if (totalTicks <= 0) totalTicks = 1;
    healPerTick = healAmount / totalTicks;

    qDebug() << "Starting continuous heal:" << healAmount << "over" << duration << "ms";
    qDebug() << "Heal per tick:" << healPerTick << "Total ticks:" << totalTicks;

    // 创建持续治疗定时器
    if (!continuousHealTimer) {
        continuousHealTimer = new QTimer(this);
        connect(continuousHealTimer, &QTimer::timeout, this, &MedicalItem::onContinuousHealTick);
    }

    continuousHealTimer->start(healTickInterval);

    // 设置总持续时间定时器
    QTimer::singleShot(duration, this, [this]() {
        if (continuousHealTimer) {
            continuousHealTimer->stop();
            qDebug() << "Continuous heal completed";
        }
    });
}

void MedicalItem::applySpeedBoost(Character* character, qreal speedBoost, int duration)
{
    if (!character || speedBoost <= 0.0 || duration <= 0) return;

    speedTarget = character;
    originalSpeed = character->getMoveSpeed();
    boostMultiplier = speedBoost;

    // 应用速度加成
    character->setMoveSpeed(originalSpeed * boostMultiplier);

    qDebug() << "Applied speed boost:" << speedBoost << "for" << duration << "ms";
    qDebug() << "Original speed:" << originalSpeed << "New speed:" << character->getMoveSpeed();

    // 创建速度加成定时器
    if (!speedBoostTimer) {
        speedBoostTimer = new QTimer(this);
        connect(speedBoostTimer, &QTimer::timeout, this, &MedicalItem::onSpeedBoostEnd);
    }

    speedBoostTimer->setSingleShot(true);
    speedBoostTimer->start(duration);
}

void MedicalItem::markUsedAndDestroy()
{
    if (used) return;

    used = true;
    qDebug() << "Medical item used, scheduling destruction";

    // 设置清理定时器，延迟删除以确保所有效果都能正常应用
    if (!cleanupTimer) {
        cleanupTimer = new QTimer(this);
        connect(cleanupTimer, &QTimer::timeout, this, &MedicalItem::onCleanupTimer);
    }

    cleanupTimer->setSingleShot(true);
    cleanupTimer->start(100); // 100ms后删除
}

void MedicalItem::onContinuousHealTick()
{
    if (!healTarget || remainingHeal <= 0) {
        if (continuousHealTimer) {
            continuousHealTimer->stop();
        }
        return;
    }

    // 应用这一轮的治疗
    int actualHeal = qMin(healPerTick, remainingHeal);
    healTarget->heal(actualHeal);
    remainingHeal -= actualHeal;

    qDebug() << "Continuous heal tick:" << actualHeal << "Remaining:" << remainingHeal;

    // 如果治疗完成，停止定时器
    if (remainingHeal <= 0 && continuousHealTimer) {
        continuousHealTimer->stop();
        qDebug() << "Continuous heal completed";
    }
}

void MedicalItem::onSpeedBoostEnd()
{
    if (speedTarget) {
        // 恢复原始速度
        speedTarget->setMoveSpeed(originalSpeed);
        qDebug() << "Speed boost ended, restored speed to:" << originalSpeed;
        speedTarget = nullptr;
    }
}

void MedicalItem::onCleanupTimer()
{
    qDebug() << "Cleaning up medical item";

    // 从场景中移除
    if (scene()) {
        scene()->removeItem(this);
    }

    // 使用安全删除
    this->deleteLater();
}
