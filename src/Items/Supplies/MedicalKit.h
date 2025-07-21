#ifndef MEDICALKIT_H
#define MEDICALKIT_H

#include "MedicalSupply.h"


class MedicalKit : public MedicalItem
{
    Q_OBJECT

public:

    explicit MedicalKit(QGraphicsItem *parent = nullptr);
    bool ApplytoCharacter(Character* character) override;
    QString getMedicalItemName() const override { return "末日医疗箱"; }
    QString getDescription() const override { return "这只是一个平平无奇的医疗箱罢了（可以立即恢复全部血量）"; }
    int getInstantHeal() const override { return HEAL_AMOUNT; }
    void mountToParent() override;
    void unmount() override;

private:
    static const int HEAL_AMOUNT = 100;  // 治疗量：100血量（全血）
    static const QString PIXMAP_PATH;    // 图片路径
};

#endif // MEDICALKIT_H
