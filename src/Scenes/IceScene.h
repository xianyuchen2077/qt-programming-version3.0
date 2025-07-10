#ifndef ICESCENE_H
#define ICESCENE_H


#include <QKeyEvent>
#include "Scene.h"
#include "../Items/Maps/Map.h"
#include "../Items/Characters/Link.h"
#include <QList>
#include <QTimer>

class IceScene : public Scene
{
    Q_OBJECT

public:
    explicit IceScene(QObject *parent);

    void processInput() override;

    void processMovement() override;

    void processPicking() override;

protected slots:

    void update() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

private:

    Mountable *findNearestUnmountedMountable(const QPointF &pos, qreal distance_threshold = std::numeric_limits<qreal>::max());

    static Mountable * pickupMountable(Character *character, Mountable *mountable);

    // --- 新属性在此处添加 ---
    Map *map;
    Link *player1;
    Link *player2;
    Armor *spareArmor;
    HeadEquipment *spareHeadEquipment;

    // QList<Character *> enemies; // 示例：一个用于存放多个敌人的列表
    // QTimer *snowfallTimer;     // 示例：一个用于管理下雪效果的定时器
    // int score;                 // 示例：一个用于跟踪玩家在该场景中分数的整数
};


#endif // ICESCENE_H
