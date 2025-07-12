#ifndef ICESCENE_H
#define ICESCENE_H

#include <QKeyEvent>
#include <QTimer>
#include <QElapsedTimer>
#include "Scene.h"
#include "../Items/Maps/Map.h"
#include "../Items/Characters/Link.h"
#include <QList>

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
    void gameLoop(); // 新增：游戏主循环

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    Mountable *findNearestUnmountedMountable(const QPointF &pos, qreal distance_threshold = std::numeric_limits<qreal>::max());
    static Mountable * pickupMountable(Character *character, Mountable *mountable);

    Map *map;
    Link *player1;
    Link *player2;
    Armor *spareArmor;
    HeadEquipment *spareHeadEquipment;

    // 新增：游戏循环相关
    QTimer *gameTimer;              // 游戏主循环定时器
    QElapsedTimer elapsedTimer;     // 用于计算帧间隔
    qint64 lastFrameTime;           // 上一帧的时间

    static const int TARGET_FPS = 60;  // 目标帧率
    static const int FRAME_TIME = 1000 / TARGET_FPS;  // 每帧时间（毫秒）

    // QList<Character *> enemies; // 示例：一个用于存放多个敌人的列表
    // QTimer *snowfallTimer;     // 示例：一个用于管理下雪效果的定时器
    // int score;                 // 示例：一个用于跟踪玩家在该场景中分数的整数
};

#endif // ICESCENE_H
