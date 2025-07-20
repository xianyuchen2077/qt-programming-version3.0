#ifndef ADRENALINE_H
#define ADRENALINE_H

#include "MedicalSupply.h"

class Adrenaline : public MedicalItem
{
    Q_OBJECT

public:
    explicit Adrenaline(QGraphicsItem *parent = nullptr);
    bool ApplytoCharacter(Character* character) override;
    QString getName() const override { return "飙升的肾上腺素"; }
    QString getDescription() const override { return "当肉体达到极限时，肾上腺素会带你杀出重围（能够增加移动速度并持续缓慢回血）"; }
    int getInstantHeal() const override { return INSTANT_HEAL; }
    int getContinuousHeal() const override { return CONTINUOUS_HEAL; }
    int getContinuousHealDuration() const override { return HEAL_DURATION; }
    qreal getSpeedBoost() const override { return SPEED_BOOST; }
    int getSpeedBoostDuration() const override { return SPEED_DURATION; }

    void mountToParent() override;
    void unmount() override;

private:
    static const int INSTANT_HEAL = 10;        // 立即治疗量：10血量
    static const int CONTINUOUS_HEAL = 40;     // 持续治疗总量：40血量
    static const int HEAL_DURATION = 10000;    // 持续治疗时间：10秒
    static const qreal SPEED_BOOST;            // 移动速度加成：2倍
    static const int SPEED_DURATION = 8000;    // 速度加成持续时间：8秒
    static const QString PIXMAP_PATH;          // 图片路径
};

#endif // ADRENALINE_H
