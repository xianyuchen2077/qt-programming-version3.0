#include "MedicalSupply.h"
#include "../Characters/Character.h"
#include <QDebug>

MedicalItem::MedicalItem(QGraphicsItem *parent, const QString &pixmapPath)
    : Item(parent, pixmapPath)
    , used(false)
    , continuousHealTimer(nullptr)
    , speedBoostTimer(nullptr)
    , healTarget(nullptr)
    , remainingHeal(0)
    , healPerTick(0)
    , healTickInterval(1000)
    , speedTarget(nullptr)
    , originalSpeed(0.0)
    , boostMultiplier(1.0)
{

}

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

    // 如果角色仍然存在且速度被修改过，恢复原始速度
    if (speedTarget && originalSpeed > 0)
    {
        qDebug() << "Restoring speed in destructor";
        speedTarget->setMoveSpeed(originalSpeed);
        speedTarget = nullptr;
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

void MedicalItem::applySpeedBoost(Character* character, qreal speedMultiplier, int duration)
{
    if (!character)
    {
        qDebug() << "speedTarget is null, aborting";
        return;
    }

    qDebug() << "Starting speed boost application";
    qDebug() << "Duration:" << duration << "ms";
    qDebug() << "Multiplier:" << speedMultiplier;

    // 保存原始速度
    originalSpeed = character->getMoveSpeed();
    qreal localOriginalSpeed = character->getMoveSpeed();
    speedTarget = character;
    qDebug() << "Original speed:" << originalSpeed;

    // 应用速度加成
    qreal newSpeed = originalSpeed * speedMultiplier;
    character->setMoveSpeed(newSpeed);
    qDebug() << "Speed boosted from" << originalSpeed << "to" << newSpeed;

    // 如果之前有速度加成定时器，先停止旧的定时器
    if (speedBoostTimer)
    {
        speedBoostTimer->stop();
        speedBoostTimer->deleteLater();
        speedBoostTimer = nullptr;
        qDebug() << "Cleaning up old timer";
    }

    // 创建定时器来恢复原始速度
    speedBoostTimer = new QTimer(this);

    QObject::connect(speedBoostTimer, &QTimer::timeout, [this, character, localOriginalSpeed](){
        qDebug() << "[SpeedBoost] Timer timeout triggered!";

        // 使用lambda捕获的参数，更可靠
        if (character && speedTarget == character)
        {
            character->setMoveSpeed(localOriginalSpeed);
            qDebug() << "Speed restored to original:" << localOriginalSpeed;
            qDebug() << "Current character speed:" << character->getMoveSpeed();
        }
        else
        {
            qDebug() << "Character reference lost on timeout";
        }

        // 清理定时器
        if (speedBoostTimer)
        {
            speedBoostTimer->stop();
            speedBoostTimer->deleteLater();
            speedBoostTimer = nullptr;
        }
        speedTarget = nullptr;

        qDebug() << "[SpeedBoost] Timer cleanup completed";
    });

    speedBoostTimer->setSingleShot(true);  // 只触发一次
    speedBoostTimer->start(duration);
    qDebug() << "Speed boost timer started for" << duration << "ms";
}

void MedicalItem::markUsedAndDestroy()
{
    if (used) return;

    used = true;
    qDebug() << "Medical item used, scheduling destruction";

    if (speedBoostTimer && speedBoostTimer->isActive())
    {
        qDebug() << "Delaying destruction due to active speed boost";

        // 创建延迟删除定时器
        QTimer* delayTimer = new QTimer();
        delayTimer->setSingleShot(true);
        QObject::connect(delayTimer, &QTimer::timeout, [this, delayTimer](){
            qDebug() << "Delayed destruction triggered";
            delayTimer->deleteLater();
            // 在这里执行实际的清理工作
            setScale(0); // 隐藏物品
        });

        // 等待速度效果结束后再删除
        delayTimer->start(getSpeedBoostDuration() + 1000); // 多等1秒确保安全
    }
    else
    {
        // 没有活跃效果，可以立即标记为删除
        setScale(0);
    }
}

void MedicalItem::onContinuousHealTick()
{
    if (!healTarget || remainingHeal <= 0)
    {
        if (continuousHealTimer)
        {
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
    if (remainingHeal <= 0 && continuousHealTimer)
    {
        continuousHealTimer->stop();
        qDebug() << "Continuous heal completed";
    }
}
