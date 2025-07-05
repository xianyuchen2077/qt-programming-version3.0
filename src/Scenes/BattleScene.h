#ifndef QT_PROGRAMMING_2024_BATTLESCENE_H
#define QT_PROGRAMMING_2024_BATTLESCENE_H

#include <QKeyEvent>
#include "Scene.h"
#include "../Items/Maps/Map.h"
#include "../Items/Characters/Character.h"

class BattleScene : public Scene {
Q_OBJECT

public:
    explicit BattleScene(QObject *parent);

    // 重写基类Scene的虚函数，处理游戏循环的不同阶段
    void processInput() override;    // 处理场景输入
    void processMovement() override; // 处理场景内对象的移动
    void processPicking() override;  // 处理场景内物品的拾取逻辑

protected slots:

    void update() override;  // 重写基类Scene的update槽函数，驱动游戏主循环

protected:
    void keyPressEvent(QKeyEvent *event) override;  // 键盘按下事件处理
    void keyReleaseEvent(QKeyEvent *event) override;  // 键盘释放事件处理

private:

    // 查找最近的未装备的可穿戴物品的私有辅助函数
    Mountable *findNearestUnmountedMountable(const QPointF &pos, qreal distance_threshold = std::numeric_limits<qreal>::max());

    // 静态私有辅助函数，处理角色拾取可穿戴物品的通用逻辑
    static Mountable * pickupMountable(Character *character, Mountable *mountable);

    Map *map;
    Character *character;
    Armor *spareArmor;
};


#endif //QT_PROGRAMMING_2024_BATTLESCENE_H
