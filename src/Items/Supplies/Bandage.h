#ifndef BANDAGE_H
#define BANDAGE_H


#include "MedicalSupply.h"

class Bandage : public MedicalItem
{
    Q_OBJECT

public:
    explicit Bandage(QGraphicsItem *parent = nullptr);
    bool ApplytoCharacter(Character* character) override;
    QString getName() const override { return "海底小纵队皮医生的绷带"; }
    QString getDescription() const override { return "简单的医疗绷带，可以立即恢复20滴血量"; }
    int getInstantHeal() const override { return HEAL_AMOUNT; }
    void mountToParent() override;
    void unmount() override;

private:
    static const int HEAL_AMOUNT = 20;  // 治疗量：20血量
    static const QString PIXMAP_PATH;   // 图片路径
};


#endif // BANDAGE_H
