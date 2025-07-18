#ifndef MEDICALSUPPLY_H
#define MEDICALSUPPLY_H


#include "../Item.h"
#include <QTimer>

class Character;


// 疗物品基类
class MedicalItem : public Item
{
    Q_OBJECT

public:
    explicit MedicalItem(QGraphicsItem *parent, const QString &pixmapPath);
    virtual ~MedicalItem();

    // 医疗物品是否成功应用到角色
    virtual bool ApplytoCharacter(Character* character) = 0;

    // 获取医疗物品名称
    virtual QString getName() const = 0;

    // 获取医疗物品描述
    virtual QString getDescription() const = 0;

    // 获取立即治疗量
    virtual int getInstantHeal() const = 0;

    // 获取持续治疗总量
    virtual int getContinuousHeal() const { return 0; }

    // 获取持续治疗时间（毫秒）
    virtual int getContinuousHealDuration() const { return 0; }

    // 获取移动速度加成
    virtual qreal getSpeedBoost() const { return 0.0; }

    // 获取移动速度加成持续时间（毫秒）
    virtual int getSpeedBoostDuration() const { return 0; }

    // 是否已被使用
    bool isUsed() const { return used; }

protected:
    // 应用立即治疗效果
    void applyInstantHeal(Character* character, int healAmount);

    // 应用持续治疗效果
    void applyContinuousHeal(Character* character, int healAmount, int duration);

    // 应用移动速度加成
    void applySpeedBoost(Character* character, qreal speedBoost, int duration);

    // 标记物品已使用并安排删除
    void markUsedAndDestroy();

private slots:
    // 持续治疗定时器槽函数
    void onContinuousHealTick();

    // 移动速度加成结束槽函数
    void onSpeedBoostEnd();

    // 清理定时器槽函数
    void onCleanupTimer();

private:
    bool used;                          // 是否已被使用
    QTimer* continuousHealTimer;        // 持续治疗定时器
    QTimer* speedBoostTimer;            // 速度加成定时器
    QTimer* cleanupTimer;               // 清理定时器

    // 持续治疗相关
    Character* healTarget;              // 治疗目标
    int remainingHeal;                  // 剩余治疗量
    int healPerTick;                    // 每次治疗量
    int healTickInterval;               // 治疗间隔（毫秒）

    // 速度加成相关
    Character* speedTarget;             // 速度加成目标
    qreal originalSpeed;                // 原始移动速度
    qreal boostMultiplier;              // 速度加成倍数
};


#endif // MEDICALSUPPLY_H
