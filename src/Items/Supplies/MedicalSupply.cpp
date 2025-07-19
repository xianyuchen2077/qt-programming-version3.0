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

// 实现 paint 方法
void MedicalItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // 首先调用父类的 paint 方法，以确保 MedicalItem 自身的 pixmapItem 被绘制
    Item::paint(painter, option, widget);

    // 如果调试绘制标志为真，则绘制中心点
    if (m_drawDebugCenter)
    {
        painter->save();
        // 保存当前绘图器状态

        // 启用抗锯齿，使绘制的圆更平滑
        painter->setRenderHint(QPainter::Antialiasing);

        // 设置画笔颜色为红色，粗细为2
        QPen pen(Qt::red);
        pen.setWidth(4);
        painter->setPen(pen);

        // 设置填充颜色为红色
        painter->setBrush(Qt::red);

        // 绘制一个小的红色圆圈在 (0,0) 位置
        // 圆圈的半径设为3个像素，你可以根据需要调整
        qreal radius = 5.0;
        painter->drawEllipse(QPointF(0, 0), radius, radius);

        painter->restore();
        // 恢复之前保存的绘图器状态
    }
}

// 实现设置调试绘制标志的方法
void MedicalItem::setDrawDebugCenter(bool draw)
{
    m_drawDebugCenter = draw;
    update();
    // 请求重新绘制，使更改立即生效
}
